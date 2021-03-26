//! The functions within Knight.

use crate::{Value, RuntimeError, Number};
use std::fmt::{self, Debug, Formatter};
use std::hash::{Hash, Hasher};
use std::collections::HashMap;
use parking_lot::Mutex;

type FuncPtr = fn(&[Value]) -> Result<Value, RuntimeError>;

/// The function type within Knight.
///
/// This struct is used to keep track of the arity and name of the function, as well as a pointer to it.
/// 
/// `Function` cannot be created directly; you must use the [`Function::register`] and [`Function::fetch`] functions.
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

		debug_assert_eq!(func_eql, self.name == rhs.name,
			"name and functions don't match up\nself={:?}\nrhs={:?}", self, rhs);

		return func_eql;
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
	pub fn func(&self) -> FuncPtr {
		self.func
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
pub fn prompt(_: &[Value]) -> Result<Value, RuntimeError> {
	let mut buf = String::new();

	io::stdin().read_line(&mut buf)?;

	Ok(buf.into())
}

pub fn random(_: &[Value]) -> Result<Value, RuntimeError> {
	Ok(rand::random::<Number>().into())
}

// arity one

pub fn eval(args: &[Value]) -> Result<Value, RuntimeError> {
	crate::run_str(&args[0].to_rcstr()?)
}

pub fn block(args: &[Value]) -> Result<Value, RuntimeError> {
	Ok(args[0].clone())
}

pub fn call(args: &[Value]) -> Result<Value, RuntimeError> {
	args[0].run()?.run()
}

#[cfg(not(feature = "embedded"))]
pub fn system(args: &[Value]) -> Result<Value, RuntimeError> {
	process::Command::new("sh")
		.arg("-c")
		.arg(&*args[0].to_rcstr()?)
		.output()
		.map(|out| String::from_utf8_lossy(&out.stdout).into_owned())
		.map_err(From::from)
		.map(Value::from)
}

#[cfg(not(feature = "embedded"))]
pub fn quit(args: &[Value]) -> Result<Value, RuntimeError> {
	process::exit(args[0].to_number()? as i32);
}

pub fn not(args: &[Value]) -> Result<Value, RuntimeError> {
	Ok((!args[0].to_boolean()?).into())
}

pub fn length(args: &[Value]) -> Result<Value, RuntimeError> {
	args[0].to_rcstr()
		.map(|rcstr| rcstr.len() as Number)
		.map(Value::from)
}

#[cfg(not(feature = "embedded"))]
pub fn dump(args: &[Value]) -> Result<Value, RuntimeError> {
	let ret = args[0].run()?;

	println!("{:?}", ret);

	Ok(ret)
}

#[cfg(not(feature = "embedded"))]
pub fn output(args: &[Value]) -> Result<Value, RuntimeError> {
	let text = args[0].to_rcstr()?;

	if let Some(stripped) = text.strip_suffix('\\') {
		print!("{}", stripped);

		io::stdout().flush()?;
	} else {
		println!("{}", text);
	}

	Ok(Value::default())
}

// arity two

pub fn add(args: &[Value]) -> Result<Value, RuntimeError> {
	args[0].run()?.try_add(&args[1].run()?)
}

pub fn subtract(args: &[Value]) -> Result<Value, RuntimeError> {
	args[0].run()?.try_sub(&args[1].run()?)
}

pub fn multiply(args: &[Value]) -> Result<Value, RuntimeError> {
	args[0].run()?.try_mul(&args[1].run()?)
}

pub fn divide(args: &[Value]) -> Result<Value, RuntimeError> {
	args[0].run()?.try_div(&args[1].run()?)
}

pub fn modulo(args: &[Value]) -> Result<Value, RuntimeError> {
	args[0].run()?.try_rem(&args[1].run()?)
}

pub fn power(args: &[Value]) -> Result<Value, RuntimeError> {
	args[0].run()?.try_pow(&args[1].run()?)
}

pub fn equals(args: &[Value]) -> Result<Value, RuntimeError> {
	args[0].run()?.try_eql(&args[1].run()?).map(Value::from)
}

pub fn less_than(args: &[Value]) -> Result<Value, RuntimeError> {
	args[0].run()?.try_lth(&args[1].run()?).map(Value::from)
}

pub fn greater_than(args: &[Value]) -> Result<Value, RuntimeError> {
	args[0].run()?.try_gth(&args[1].run()?).map(Value::from)
}

pub fn and(args: &[Value]) -> Result<Value, RuntimeError> {
	let lhs = args[0].run()?;

	if lhs.to_boolean()? {
		args[1].run()
	} else {
		Ok(lhs)
	}
}

pub fn or(args: &[Value]) -> Result<Value, RuntimeError> {
	let lhs = args[0].run()?;

	if lhs.to_boolean()? {
		Ok(lhs)
	} else {
		args[1].run()
	}
}

pub fn then(args: &[Value]) -> Result<Value, RuntimeError> {
	args[0].run()?;
	args[1].run()
}

pub fn assign(args: &[Value]) -> Result<Value, RuntimeError> {
	let variable = 
		if let Value::Variable(ref variable) = args[0] {
			variable
		} else {
			return Err(RuntimeError::InvalidOperand { func: '?', operand: args[0].typename() });
		};

	let rhs = args[1].run()?;

	variable.assign(rhs.clone());

	Ok(rhs)
}

pub fn r#while(args: &[Value]) -> Result<Value, RuntimeError> {
	while args[0].to_boolean()? {
		args[1].run()?;
	}

	Ok(Value::Null)
}

// arity three

pub fn r#if(args: &[Value]) -> Result<Value, RuntimeError> {
	if args[0].to_boolean()? {
		args[1].run()
	} else {
		args[2].run()
	}
}

pub fn get(args: &[Value]) -> Result<Value, RuntimeError> {
	Ok(Value::String(
		args[0]
			.to_rcstr()?
			.chars()
			.skip(args[1].to_number()? as usize)
			.take(args[2].to_number()? as usize)
			.collect::<String>()
			.into()
	))
}

// arity four

pub fn substitute(args: &[Value]) -> Result<Value, RuntimeError> {
	let s = args[0].to_rcstr()?;
	let start = args[1].to_number()? as usize;
	let stop = start + args[2].to_number()? as usize;

	let mut x = s.chars().take(start).collect::<String>();
	x.push_str(&args[3].to_rcstr()?);
	x.extend(s.chars().skip(stop));

	Ok(Value::String(x.into()))
}
