//! The functions within Knight.

use crate::{Value, RuntimeError, Number, Environment, RcString};
use std::fmt::{self, Debug, Formatter};
use std::hash::{Hash, Hasher};
use std::collections::HashMap;
use std::convert::{TryInto, TryFrom};
use std::sync::Mutex;

// An alias to make life easier.
type FuncPtr = fn(&[Value], &mut Environment<'_, '_, '_>) -> Result<Value, RuntimeError>;

/// The type that represents functions themselves (eg `PROMPT`, `+`, `=`, etc.) within Knight.
/// 
/// Note that [`Function`]s cannot be created directly---you must [`fetch`](Function::fetch) them. New functions can be
/// [`register`](Function::register)ed if so desired.
#[derive(Clone, Copy)]
pub struct Function {
	func: FuncPtr,
	name: char,
	arity: usize
}

impl Debug for Function {
	fn fmt(&self, f: &mut Formatter<'_>) -> fmt::Result {
		struct PointerDebug(usize);

		impl Debug for PointerDebug {
			fn fmt(&self, f: &mut Formatter<'_>) -> fmt::Result {
				write!(f, "{:p}", self.0 as *const ())
			}
		}

		if f.alternate() {
			f.debug_struct("Function")
				.field("func", &PointerDebug(self.func as usize))
				.field("name", &self.name)
				.field("arity", &self.arity)
				.finish()
		} else {
			f.debug_tuple("function")
				.field(&self.name)
				.finish()
		}
	}
}

impl Eq for Function {}
impl PartialEq for Function {
	/// Checks to see if two functions are identical.
	///
	/// Two functions are considered the same if their names, arities, and function pointers are identical.
	fn eq(&self, rhs: &Self) -> bool {
		self.name == rhs.name
			&& (self.func as usize) == (rhs.func as usize)
			&& self.arity == rhs.arity
	}
}

impl Hash for Function {
	fn hash<H: Hasher>(&self, h: &mut H) {
		(self.func as usize).hash(h);
	}
}

impl Function {
	/// Gets the function pointer associated with `self`.
	#[inline]
	#[must_use]
	pub fn func(&self) -> FuncPtr {
		self.func
	}

	/// Executes this function with the given arguments
	pub fn run(&self, args: &[Value], env: &mut Environment<'_, '_, '_>) -> Result<Value, RuntimeError> {
		(self.func)(args, env)
	}

	/// Gets the arity of this function.
	#[inline]
	#[must_use]
	pub fn arity(&self) -> usize {
		self.arity
	}

	/// Gets the name of the function.
	#[inline]
	#[must_use]
	pub fn name(&self) -> char {
		self.name
	}

	/// Gets the function associate dwith the given `name`, returning `None` if no such function exists.
	#[must_use = "fetching a function does nothing by itself"]
	pub fn fetch(name: char) -> Option<Self> {
		FUNCTIONS.lock().unwrap().get(&name).cloned()
	}

	/// Registers a new function with the given name, discarding any previous value associated with it.
	pub fn register(name: char, arity: usize, func: FuncPtr) {
		FUNCTIONS.lock().unwrap().insert(name, Self { name, arity, func });
	}
}

lazy_static::lazy_static! {
	static ref FUNCTIONS: Mutex<HashMap<char, Function>> = Mutex::new({
		let mut map = HashMap::new();

		macro_rules! insert {
			($name:expr, $arity:expr, $func:expr) => {
				map.insert($name, Function { name: $name, arity: $arity, func: $func });
			};
		}

		insert!('P', 0, prompt);
		insert!('R', 0, random);

		insert!('E', 1, eval);
		insert!('B', 1, block);
		insert!('C', 1, call);
		insert!('`', 1, system);
		insert!('Q', 1, quit);
		insert!('!', 1, not);
		insert!('L', 1, length);
		insert!('D', 1, dump);
		insert!('O', 1, output);

		insert!('+', 2, add);
		insert!('-', 2, subtract);
		insert!('*', 2, multiply);
		insert!('/', 2, divide);
		insert!('%', 2, modulo);
		insert!('^', 2, power);
		insert!('?', 2, equals);
		insert!('<', 2, less_than);
		insert!('>', 2, greater_than);
		insert!('&', 2, and);
		insert!('|', 2, or);
		insert!(';', 2, then);
		insert!('=', 2, assign);
		insert!('W', 2, r#while);

		insert!('I', 3, r#if);
		insert!('G', 3, get);
		insert!('S', 4, substitute);

		map
	});
}

use std::io::{Write, BufRead};

// arity zero
pub fn prompt(_: &[Value], env: &mut Environment<'_, '_, '_>) -> Result<Value, RuntimeError> {
	let mut buf = String::new();

	std::io::BufReader::new(env).read_line(&mut buf)?;

	RcString::try_from(buf).map(From::from).map_err(From::from)
}

pub fn random(_: &[Value], _: &mut Environment<'_, '_, '_>) -> Result<Value, RuntimeError> {
	Ok(rand::random::<Number>().into())
}

// arity one

pub fn eval(args: &[Value], env: &mut Environment<'_, '_, '_>) -> Result<Value, RuntimeError> {
	crate::run_str(&args[0].run(env)?.to_rcstring()?, env)
}

pub fn block(args: &[Value], _: &mut Environment<'_, '_, '_>) -> Result<Value, RuntimeError> {
	Ok(args[0].clone())
}

pub fn call(args: &[Value], env: &mut Environment<'_, '_, '_>) -> Result<Value, RuntimeError> {
	args[0].run(env)?.run(env)
}

pub fn system(args: &[Value], env: &mut Environment<'_, '_, '_>) -> Result<Value, RuntimeError> {
	let cmd = args[0].run(env)?.to_rcstring()?;

	env.run_command(&cmd).map(Value::from)
}

pub fn quit(args: &[Value], env: &mut Environment<'_, '_, '_>) -> Result<Value, RuntimeError> {
	Err(RuntimeError::Quit(args[0].run(env)?.to_number()? as i32))
}

pub fn not(args: &[Value], env: &mut Environment<'_, '_, '_>) -> Result<Value, RuntimeError> {
	Ok((!args[0].run(env)?.to_boolean()?).into())
}

pub fn length(args: &[Value], env: &mut Environment<'_, '_, '_>) -> Result<Value, RuntimeError> {
	args[0].run(env)?.to_rcstring()
		.map(|rcstring| rcstring.len() as Number)
		.map(Value::from)
}

pub fn dump(args: &[Value], env: &mut Environment<'_, '_, '_>) -> Result<Value, RuntimeError> {
	let ret = args[0].run(env)?;

	writeln!(env, "{:?}", ret)?;

	Ok(ret)
}

pub fn output(args: &[Value], env: &mut Environment<'_, '_, '_>) -> Result<Value, RuntimeError> {
	let text = args[0].run(env)?.to_rcstring()?;

	if let Some(stripped) = text.strip_suffix('\\') {
		write!(env, "{}", stripped)?;
		env.flush()?;
	} else {
		writeln!(env, "{}", text)?;
	}

	Ok(Value::default())
}

// arity two

pub fn add(args: &[Value], env: &mut Environment<'_, '_, '_>) -> Result<Value, RuntimeError> {
	match args[0].run(env)? {
		Value::Number(lhs) => {
			let rhs = args[1].run(env)?.to_number()?;

			#[cfg(feature = "checked-overflow")]
			{ lhs.checked_add(rhs).map(Value::Number).ok_or_else(|| RuntimeError::Overflow { func: '+', lhs, rhs }) }

			#[cfg(not(feature = "checked-overflow"))]
			{ Ok(Value::Number(lhs + rhs)) }
		},
		Value::String(lhs) => {
			let rhs = args[1].run(env)?.to_rcstring()?;

			// both `RcString.to_string()` is a valid RcString, so adding it to `to_rcstring` is valid.
			Ok(Value::String(RcString::try_from(lhs.to_string() + &rhs).unwrap()))
		},
		other => Err(RuntimeError::InvalidOperand { func: '+', operand: other.typename() })
	}
}

pub fn subtract(args: &[Value], env: &mut Environment<'_, '_, '_>) -> Result<Value, RuntimeError> {
	match args[0].run(env)? {
		Value::Number(lhs) => {
			let rhs = args[1].run(env)?.to_number()?;
			
			#[cfg(feature = "checked-overflow")]
			{ lhs.checked_add(rhs).map(Value::Number).ok_or_else(|| RuntimeError::Overflow { func: '-', lhs, rhs }) }

			#[cfg(not(feature = "checked-overflow"))]
			{ Ok(Value::Number(lhs - rhs)) }
		},
		other => Err(RuntimeError::InvalidOperand { func: '-', operand: other.typename() })
	}
}

pub fn multiply(args: &[Value], env: &mut Environment<'_, '_, '_>) -> Result<Value, RuntimeError> {
	match args[0].run(env)? {
		Value::Number(lhs) => {
			let rhs = args[1].run(env)?.to_number()?;
			
			#[cfg(feature = "checked-overflow")]
			{ lhs.checked_add(rhs).map(Value::Number).ok_or_else(|| RuntimeError::Overflow { func: '*', lhs, rhs }) }

			#[cfg(not(feature = "checked-overflow"))]
			{ Ok(Value::Number(lhs * rhs)) }
		}
		Value::String(lhs) =>
			RcString::try_from(args[1].run(env)?
				.to_number()
				.map(|rhs| (0..rhs).map(|_| lhs.as_str()).collect::<String>())?)
				.map_err(From::from)
				.map(Value::String),
		other => Err(RuntimeError::InvalidOperand { func: '*', operand: other.typename() })
	}
}

pub fn divide(args: &[Value], env: &mut Environment<'_, '_, '_>) -> Result<Value, RuntimeError> {
	match args[0].run(env)? {
		Value::Number(lhs) =>
			lhs.checked_div(args[1].run(env)?.to_number()?)
				.map(Value::from)
				.ok_or(RuntimeError::DivisionByZero { modulo: false }),
		other => Err(RuntimeError::InvalidOperand { func: '/', operand: other.typename() })
	}
}

pub fn modulo(args: &[Value], env: &mut Environment<'_, '_, '_>) -> Result<Value, RuntimeError> {
	match args[0].run(env)? {
		Value::Number(lhs) =>
			lhs.checked_rem(args[1].run(env)?.to_number()?)
				.map(Value::from)
				.ok_or(RuntimeError::DivisionByZero { modulo: true }),
		other => Err(RuntimeError::InvalidOperand { func: '%', operand: other.typename() })
	}
}

// TODO: checked-overflow for this function.
pub fn power(args: &[Value], env: &mut Environment<'_, '_, '_>) -> Result<Value, RuntimeError> {
	let base = 
		match args[0].run(env)? {
			Value::Number(lhs) => lhs,
			other => return Err(RuntimeError::InvalidOperand { func: '^', operand: other.typename() })
		};

	let exponent = args[1].run(env)?.to_number()?;

	Ok(Value::Number(
		if base == 1 {
			1
		} else if base == -1 {
			if exponent & 1 == 1 {
				-1
			} else {
				1
			}
		} else {
			match exponent {
				1 => base,
				0 => 1,
				_ if exponent < 0 => 0,
				_ => base.pow(exponent as u32)
			}
		}
	))
}

pub fn equals(args: &[Value], env: &mut Environment<'_, '_, '_>) -> Result<Value, RuntimeError> {
	Ok((args[0].run(env)? == args[1].run(env)?).into())
}

pub fn less_than(args: &[Value], env: &mut Environment<'_, '_, '_>) -> Result<Value, RuntimeError> {
	match args[0].run(env)? {
		Value::Number(lhs) => Ok((lhs < args[1].run(env)?.to_number()?).into()),
		Value::Boolean(lhs) => Ok((lhs < args[1].run(env)?.to_boolean()?).into()),
		Value::String(lhs) => Ok((lhs.as_str() < args[1].run(env)?.to_rcstring()?.as_str()).into()),
		other => Err(RuntimeError::InvalidOperand { func: '<', operand: other.typename() })
	}
}

pub fn greater_than(args: &[Value], env: &mut Environment<'_, '_, '_>) -> Result<Value, RuntimeError> {
	match args[0].run(env)? {
		Value::Number(lhs) => Ok((lhs > args[1].run(env)?.to_number()?).into()),
		Value::Boolean(lhs) => Ok((lhs > args[1].run(env)?.to_boolean()?).into()),
		Value::String(lhs) => Ok((lhs.as_str() > args[1].run(env)?.to_rcstring()?.as_str()).into()),
		other => Err(RuntimeError::InvalidOperand { func: '>', operand: other.typename() })
	}
}

pub fn and(args: &[Value], env: &mut Environment<'_, '_, '_>) -> Result<Value, RuntimeError> {
	let lhs = args[0].run(env)?;

	if lhs.to_boolean()? {
		args[1].run(env)
	} else {
		Ok(lhs)
	}
}

pub fn or(args: &[Value], env: &mut Environment<'_, '_, '_>) -> Result<Value, RuntimeError> {
	let lhs = args[0].run(env)?;

	if lhs.to_boolean()? {
		Ok(lhs)
	} else {
		args[1].run(env)
	}
}

pub fn then(args: &[Value], env: &mut Environment<'_, '_, '_>) -> Result<Value, RuntimeError> {
	args[0].run(env)?;
	args[1].run(env)
}

pub fn assign(args: &[Value], env: &mut Environment<'_, '_, '_>) -> Result<Value, RuntimeError> {
	let variable = 
		if let Value::Variable(ref variable) = args[0] {
			variable
		} else /* if cfg!(feature = "assign-to-anything") */ {
			return Err(RuntimeError::InvalidOperand { func: '?', operand: args[0].typename() });
		};

	let rhs = args[1].run(env)?;

	variable.assign(rhs.clone());

	Ok(rhs)
}

pub fn r#while(args: &[Value], env: &mut Environment<'_, '_, '_>) -> Result<Value, RuntimeError> {
	while args[0].run(env)?.to_boolean()? {
		let _ = args[1].run(env)?;
	}

	Ok(Value::default())
}

// arity three

pub fn r#if(args: &[Value], env: &mut Environment<'_, '_, '_>) -> Result<Value, RuntimeError> {
	if args[0].run(env)?.to_boolean()? {
		args[1].run(env)
	} else {
		args[2].run(env)
	}
}

pub fn get(args: &[Value], env: &mut Environment<'_, '_, '_>) -> Result<Value, RuntimeError> {
	let substr =
		args[0]
			.run(env)?
			.to_rcstring()?
			.chars()
			.skip(args[1].run(env)?.to_number()? as usize)
			.take(args[2].run(env)?.to_number()? as usize)
			.collect::<String>()
			.try_into()
			.unwrap(); // we know the substring is valid, as the source string was valid.

	Ok(Value::String(substr))
}

// arity four

pub fn substitute(args: &[Value], env: &mut Environment<'_, '_, '_>) -> Result<Value, RuntimeError> {
	let source = args[0].run(env)?.to_rcstring()?;
	let start = args[1].run(env)?.to_number()? as usize;
	let stop = start + args[2].run(env)?.to_number()? as usize;

	let mut x = source.chars().take(start).collect::<String>();
	x.push_str(&args[3].run(env)?.to_rcstring()?);
	x.extend(source.chars().skip(stop));

	Ok(Value::String(x.try_into().unwrap())) // we know the replacement is valid, as both sources were valid.
}
