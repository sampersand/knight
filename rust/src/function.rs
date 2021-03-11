use crate::{Value, RuntimeError, Number};
use std::fmt::{self, Debug, Formatter};
use std::hash::{Hash, Hasher};
use std::collections::HashMap;
use parking_lot::Mutex;

type FuncPtr = fn(&[Value]) -> Result<Value, RuntimeError>;

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

		f.debug_tuple("Function")
			.field(&PointerDebug(self.func as usize))
			.field(&self.name)
			.field(&self.arity)
			.finish()
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
	pub fn func(&self) -> FuncPtr {
		self.func
	}

	pub fn arity(&self) -> usize {
		self.arity
	}

	pub fn name(&self) -> char {
		self.name
	}

	pub fn fetch(name: char) -> Option<Self> {
		FUNCTIONS.lock().get(&name).cloned()
	}

	pub fn register(name: char, arity: usize, func: FuncPtr) {
		FUNCTIONS.lock().insert(name, Function { name, arity, func });
	}
}

macro_rules! declare_functions {
	(ARITY;) => { 0 };
	(ARITY; $_name:ident $($rest:tt)*) => { 1 +  declare_functions!(ARITY; $($rest)*) };

	($( $(#[$meta:meta])* fn $name:literal ($($arg:ident),*) $body:block  )+) => {
		lazy_static::lazy_static! {
			static ref FUNCTIONS: Mutex<HashMap<char, Function>> = Mutex::new({
				let mut map = HashMap::new();

				$(
					$(#[$meta])*
					map.insert($name, Function {
						name: $name,
						arity: declare_functions!(ARITY; $($arg)*),
						func: |args|
							if let [$($arg),*] = args {
								$body
							} else {
								unreachable!("invalid args length passed to {:?}: {:?}", args.len(), $name)
							}
					});
				)+
				map
			});
		}
	};
}

use std::io::{self, Write};
use std::process;

declare_functions! {
	// arity zero

	#[cfg(not(feature = "embedded"))]
	fn 'P' () {
		let mut buf = String::new();

		io::stdin().read_line(&mut buf)?;

		Ok(buf.into())
	}

	fn 'R'() {
		Ok(rand::random::<Number>().into())
	}

	// arity one.

	fn 'E' (arg) {
		arg.to_rcstr()
			.and_then(|arg| crate::run_str(&arg))
	}

	fn 'B' (block) {
		Ok(block.clone())
	}

	fn 'C' (block) {
		block.run()?.run()
	}

	#[cfg(not(feature = "embedded"))]
	fn '`' (cmd) {
		process::Command::new("sh")
			.arg("-c")
			.arg(&*cmd.to_rcstr()?)
			.output()
			.map(|out| String::from_utf8_lossy(&out.stdout).into_owned())
			.map_err(From::from)
			.map(Value::from)
	}

	#[cfg(not(feature = "embedded"))]
	fn 'Q' (code) {
		process::exit(code.to_number()? as i32);
	}

	fn '!' (code) {
		code.to_boolean()
			.map(|boolean| !boolean)
			.map(Value::from)
	}

	fn 'L' (string) {
		string.to_rcstr()
			.map(|rcstr| rcstr.len() as Number)
			.map(Value::from)
	}

	#[cfg(not(feature = "embedded"))]
	fn 'D' (value) {
		let ret = value.run()?;

		println!("{:?}", ret);

		Ok(ret)
	}

	#[cfg(not(feature = "embedded"))]
	fn 'O' (msg) {
		let text = msg.to_rcstr()?;

		if let Some(stripped) = text.strip_suffix('\\') {
			print!("{}", stripped);

			let _ = io::stdout().flush();
		} else {
			println!("{}", text);
		}

		Ok(Value::default())
	}

	// arity two

	fn '+' (lhs, rhs) {
		lhs.run()?.try_add(&rhs.run()?)
	}

	fn '-' (lhs, rhs) {
		lhs.run()?.try_sub(&rhs.run()?)
	}

	fn '*' (lhs, rhs) {
		lhs.run()?.try_mul(&rhs.run()?)
	}

	fn '/' (lhs, rhs) {
		lhs.run()?.try_div(&rhs.run()?)
	}

	fn '%' (lhs, rhs) {
		lhs.run()?.try_rem(&rhs.run()?)
	}

	fn '^' (lhs, rhs) {
		lhs.run()?.try_pow(&rhs.run()?)
	}

	fn '?' (lhs, rhs) {
		lhs.run()?.try_eql(&rhs.run()?).map(Value::from)
	}

	fn '<' (lhs, rhs) {
		lhs.run()?.try_lth(&rhs.run()?).map(Value::from)
	}

	fn '>' (lhs, rhs) {
		lhs.run()?.try_gth(&rhs.run()?).map(Value::from)
	}

	fn '&' (lhs, rhs) {
		let lhs = lhs.run()?;

		if lhs.to_boolean()? {
			rhs.run()
		} else {
			Ok(lhs)
		}
	}

	fn '|' (lhs, rhs) {
		let lhs = lhs.run()?;

		if lhs.to_boolean()? {
			Ok(lhs)
		} else {
			rhs.run()
		}
	}

	fn ';' (lhs, rhs) {
		lhs.run()?;
		rhs.run()
	}

	fn '=' (arg, rhs) {
		let rhsval;

		if let Value::Variable(ref ident) = arg {
			rhsval = rhs.run()?;

			crate::env::insert(ident, rhsval.clone());
		} else {
			let ident = arg.to_rcstr()?;
			rhsval = rhs.run()?;

			crate::env::insert(&ident, rhsval.clone());
		}

		Ok(rhsval)
	}

	fn 'W' (lhs, rhs) {
		while lhs.to_boolean()? {
			rhs.run()?;
		}

		Ok(Value::Null)
	}

	// arity three

	fn 'I' (cond, iftrue, iffalse) {
		if cond.to_boolean()? {
			iftrue.run()
		} else {
			iffalse.run()
		}
	}

	fn 'G' (string, start, length) {
		Ok(Value::String(
			string
				.to_rcstr()?
				.chars()
				.skip(start.to_number()? as usize)
				.take(length.to_number()? as usize)
				.collect::<String>()
				.into()
		))
	}

	// arity four

	fn 'S' (string, start, length, repl) {
		let s = string.to_rcstr()?;
		let start = start.to_number()? as usize;
		let stop = start + length.to_number()? as usize;

		let mut x = s.chars().take(start).collect::<String>();
		x.push_str(&repl.to_rcstr()?);
		x.extend(s.chars().skip(stop));

		Ok(Value::String(x.into()))
	}

}
