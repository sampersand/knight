//! The functions within Knight.

use crate::{Value, RuntimeError, Number, Environment, RcStr};
use std::fmt::{self, Debug, Formatter};
use std::hash::{Hash, Hasher};
use std::collections::HashMap;
use parking_lot::Mutex;
use std::convert::{TryInto, TryFrom};

// An alias to make life easier.
type FuncPtr = fn(&[Value], &Environment) -> Result<Value, RuntimeError>;

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
	fn fmt(&self, f: &mut Formatter) -> fmt::Result {
		struct PointerDebug(usize);

		impl Debug for PointerDebug {
			fn fmt(&self, f: &mut Formatter) -> fmt::Result {
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
	fn eq(&self, rhs: &Self) -> bool {
		let func_eql = (self.func as usize) == (rhs.func as usize);

		if cfg!(debug_assertions) && func_eql {
			debug_assert_eq!(self.name, rhs.name, "`name` and `func` mismatch\nself={:#?}\nrhs={:#?}", self, rhs);
		}

		return self.name == rhs.name;
	}
}

impl Hash for Function {
	fn hash<H: Hasher>(&self, h: &mut H) {
		(self.func as usize).hash(h);
	}
}

impl Function {
	/// Gets the function pointer associated with `self`.
	///
	/// # Examples
	/// ```rust
	/// # use knight::{Function, Value, RuntimeError, Environment};
	/// fn foo(var: &[Value], _: &Environment) -> Result<Value, RuntimeError> { Ok(args[0].clone()) }
	/// Function::register('F', 1, foo);
	/// 
	/// assert_eq!(Function::fetch('F').unwrap().func(), foo);
	/// ```
	#[inline]
	pub fn func(&self) -> FuncPtr {
		self.func
	}

	pub fn run(&self, args: &[Value], env: &Environment) -> Result<Value, RuntimeError> {
		(self.func)(args, env)
	}

	/// Gets the arity of this function.
	#[inline]
	pub fn arity(&self) -> usize {
		self.arity
	}

	/// Gets the name of the function.
	#[inline]
	pub fn name(&self) -> char {
		self.name
	}

	/// Gets the function associate dwith the given `name`, returning `None` if no such function exists.
	pub fn fetch(name: char) -> Option<Self> {
		FUNCTIONS.lock().get(&name).cloned()
	}

	/// Registers a new function with the given name, discarding any previous value associated with it.
	pub fn register(name: char, arity: usize, func: FuncPtr) {
		FUNCTIONS.lock().insert(name, Function { name, arity, func });
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

use std::io::{self, Write};
use std::process;

// arity zero

#[cfg(not(feature = "embedded"))]
pub fn prompt(_: &[Value], _: &Environment) -> Result<Value, RuntimeError> {
	let mut buf = String::new();

	io::stdin().read_line(&mut buf)?;

	RcStr::try_from(buf).map(From::from).map_err(From::from)
}

pub fn random(_: &[Value], _: &Environment) -> Result<Value, RuntimeError> {
	Ok(rand::random::<Number>().into())
}

// arity one

pub fn eval(args: &[Value], env: &Environment) -> Result<Value, RuntimeError> {
	crate::run_str(&args[0].run(env)?.to_rcstr()?, env)
}

pub fn block(args: &[Value], _: &Environment) -> Result<Value, RuntimeError> {
	Ok(args[0].clone())
}

pub fn call(args: &[Value], env: &Environment) -> Result<Value, RuntimeError> {
	args[0].run(env)?.run(env)
}

#[cfg(not(feature = "embedded"))]
pub fn system(args: &[Value], env: &Environment) -> Result<Value, RuntimeError> {
	RcStr::try_from(process::Command::new("sh")
		.arg("-c")
		.arg(&*args[0].run(env)?.to_rcstr()?)
		.output()
		.map(|out| String::from_utf8_lossy(&out.stdout).into_owned())?)
		.map_err(From::from)
		.map(From::from)
}

#[cfg(not(feature = "embedded"))]
pub fn quit(args: &[Value], env: &Environment) -> Result<Value, RuntimeError> {
	process::exit(args[0].run(env)?.to_number()? as i32);
}

pub fn not(args: &[Value], env: &Environment) -> Result<Value, RuntimeError> {
	Ok((!args[0].run(env)?.to_boolean()?).into())
}

pub fn length(args: &[Value], env: &Environment) -> Result<Value, RuntimeError> {
	args[0].run(env)?.to_rcstr()
		.map(|rcstr| rcstr.len() as Number)
		.map(Value::from)
}

#[cfg(not(feature = "embedded"))]
pub fn dump(args: &[Value], env: &Environment) -> Result<Value, RuntimeError> {
	let ret = args[0].run(env)?;

	println!("{:?}", ret);

	Ok(ret)
}

#[cfg(not(feature = "embedded"))]
pub fn output(args: &[Value], env: &Environment) -> Result<Value, RuntimeError> {
	let text = args[0].run(env)?.to_rcstr()?;

	if let Some(stripped) = text.strip_suffix('\\') {
		print!("{}", stripped);

		io::stdout().flush()?;
	} else {
		println!("{}", text);
	}

	Ok(Default::default())
}

// arity two

pub fn add(args: &[Value], env: &Environment) -> Result<Value, RuntimeError> {
	match args[0].run(env)? {
		Value::Number(lhs) => Ok(Value::Number(lhs + args[1].run(env)?.to_number()?)),
		// both `rcstr.to_string()` is a valid rcstr, so adding it to `to_rcstr` is valid.
		Value::String(lhs) => Ok(Value::String((lhs.to_string() + &args[1].run(env)?.to_rcstr()?).try_into().unwrap())),
		other => Err(RuntimeError::InvalidOperand { func: '+', operand: other.typename() })
	}
}

pub fn subtract(args: &[Value], env: &Environment) -> Result<Value, RuntimeError> {
	match args[0].run(env)? {
		Value::Number(lhs) => Ok(Value::Number(lhs - args[1].run(env)?.to_number()?)),
		other => Err(RuntimeError::InvalidOperand { func: '-', operand: other.typename() })
	}
}

pub fn multiply(args: &[Value], env: &Environment) -> Result<Value, RuntimeError> {
	match args[0].run(env)? {
		Value::Number(lhs) => Ok(Value::Number(lhs * args[1].run(env)?.to_number()?)),
		Value::String(lhs) =>
			RcStr::try_from(args[1].run(env)?
				.to_number()
				.map(|rhs| (0..rhs).map(|_| lhs.as_str()).collect::<String>())?)
				.map_err(From::from)
				.map(Value::String),
		other => Err(RuntimeError::InvalidOperand { func: '*', operand: other.typename() })
	}

}

pub fn divide(args: &[Value], env: &Environment) -> Result<Value, RuntimeError> {
	match args[0].run(env)? {
		Value::Number(lhs) =>
			match args[1].run(env)?.to_number()? { // todo: checked div?
				0 => Err(RuntimeError::DivisionByZero { modulo: false }),
				rhs => Ok(Value::Number(lhs / rhs))
			},
		other => Err(RuntimeError::InvalidOperand { func: '/', operand: other.typename() })
	}
}

pub fn modulo(args: &[Value], env: &Environment) -> Result<Value, RuntimeError> {
	match args[0].run(env)? {
		Value::Number(lhs) =>
			match args[1].run(env)?.to_number()? { // todo: checked div?
				0 => Err(RuntimeError::DivisionByZero { modulo: true }),
				rhs => Ok(Value::Number(lhs % rhs))
			},
		other => Err(RuntimeError::InvalidOperand { func: '%', operand: other.typename() })
	}
}

pub fn power(args: &[Value], env: &Environment) -> Result<Value, RuntimeError> {
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

pub fn equals(args: &[Value], env: &Environment) -> Result<Value, RuntimeError> {
	return Ok((args[0].run(env)? == args[1].run(env)?).into())
}

pub fn less_than(args: &[Value], env: &Environment) -> Result<Value, RuntimeError> {
	match args[0].run(env)? {
		Value::Number(lhs) => Ok((lhs < args[1].run(env)?.to_number()?).into()),
		Value::Boolean(lhs) => Ok((lhs < args[1].run(env)?.to_boolean()?).into()),
		Value::String(lhs) => Ok((lhs.as_str() < args[1].run(env)?.to_rcstr()?.as_str()).into()),
		other => Err(RuntimeError::InvalidOperand { func: '<', operand: other.typename() })
	}
}

pub fn greater_than(args: &[Value], env: &Environment) -> Result<Value, RuntimeError> {
	match args[0].run(env)? {
		Value::Number(lhs) => Ok((lhs > args[1].run(env)?.to_number()?).into()),
		Value::Boolean(lhs) => Ok((lhs > args[1].run(env)?.to_boolean()?).into()),
		Value::String(lhs) => Ok((lhs.as_str() > args[1].run(env)?.to_rcstr()?.as_str()).into()),
		other => Err(RuntimeError::InvalidOperand { func: '>', operand: other.typename() })
	}
}

pub fn and(args: &[Value], env: &Environment) -> Result<Value, RuntimeError> {
	let lhs = args[0].run(env)?;

	if lhs.to_boolean()? {
		args[1].run(env)
	} else {
		Ok(lhs)
	}
}

pub fn or(args: &[Value], env: &Environment) -> Result<Value, RuntimeError> {
	let lhs = args[0].run(env)?;

	if lhs.to_boolean()? {
		Ok(lhs)
	} else {
		args[1].run(env)
	}
}

pub fn then(args: &[Value], env: &Environment) -> Result<Value, RuntimeError> {
	args[0].run(env)?;
	args[1].run(env)
}

pub fn assign(args: &[Value], env: &Environment) -> Result<Value, RuntimeError> {
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

pub fn r#while(args: &[Value], env: &Environment) -> Result<Value, RuntimeError> {
	while args[0].run(env)?.to_boolean()? {
		let _ = args[1].run(env)?;
	}

	Ok(Default::default())
}

// arity three

pub fn r#if(args: &[Value], env: &Environment) -> Result<Value, RuntimeError> {
	if args[0].run(env)?.to_boolean()? {
		args[1].run(env)
	} else {
		args[2].run(env)
	}
}

pub fn get(args: &[Value], env: &Environment) -> Result<Value, RuntimeError> {
	let substr =
		args[0]
			.run(env)?
			.to_rcstr()?
			.chars()
			.skip(args[1].run(env)?.to_number()? as usize)
			.take(args[2].run(env)?.to_number()? as usize)
			.collect::<String>()
			.try_into()
			.unwrap(); // we know the substring is valid, as the source string was valid.

	Ok(Value::String(substr))
}

// arity four

pub fn substitute(args: &[Value], env: &Environment) -> Result<Value, RuntimeError> {
	let source = args[0].run(env)?.to_rcstr()?;
	let start = args[1].run(env)?.to_number()? as usize;
	let stop = start + args[2].run(env)?.to_number()? as usize;

	let mut x = source.chars().take(start).collect::<String>();
	x.push_str(&args[3].run(env)?.to_rcstr()?);
	x.extend(source.chars().skip(stop));

	Ok(Value::String(x.try_into().unwrap())) // we know the replacement is valid, as both sources were valid.
}
