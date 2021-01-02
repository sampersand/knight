use crate::{Value, Ast};
use std::fmt::{self, Debug, Formatter};

#[derive(Clone, Copy)]
pub struct Function {
	arity: usize,
	func: fn(&[Ast]) -> Value
}

impl Debug for Function {
	fn fmt(&self, f: &mut Formatter) -> fmt::Result {
		f.debug_tuple("Function")
			.field(&self.arity)
			.field(&(self.func as *const ()))
			.finish()
	}
}

impl Eq for Function {}
impl PartialEq for Function {
	fn eq(&self, rhs: &Self) -> bool {
		let ret = (self.func as usize) == (rhs.func as usize);
		debug_assert_eq!(self.arity == rhs.arity, ret);

		ret
	}
}


impl Function {
	pub fn for_name(name: u8) -> Option<Self> {
		// SAFETY: we only ever run Knight in one thread, so there's no possibility of
		// data races
		unsafe {
			FUNCTIONS[name as usize]
		}
	}

	pub fn register(name: u8, arity: usize, func: fn(&[Ast]) -> Value) {
		// SAFETY: we only ever run Knight in one thread, so there's no possibility of
		// data races
		unsafe {
			FUNCTIONS[name as usize] = Some(Function { arity, func });
		}
	}

	pub fn run(&self, args: &[Ast]) -> Value {
		(self.func)(args)
	}

	pub const fn arity(&self) -> usize {
		self.arity
	}
}

macro_rules! functions {
	(@COUNT_ARITY $arity:expr;) => { $arity };
	(@COUNT_ARITY $arity:expr; $_ident:ident $($rest:ident)*) => {
		functions!(@COUNT_ARITY $arity + 1; $($rest)*)
	};

	($(fn $name:literal ($($args:ident),*) $body:block)*) => {
		static mut FUNCTIONS: [Option<Function>; u8::MAX as usize] = {
			let mut arr = [None; u8::MAX as usize];

			$(
				arr[$name as usize] = Some(Function {
					arity: functions!(@COUNT_ARITY 0; $($args)*),
					func: |args| {
						if let [$($args),*] = args {
							$body
						} else {
							unreachable!("invalid args length given: {:?}", args.len());
						}
					}
				});
			)*
			arr
		};
	};
}

functions! {
	fn 'T'() { Value::Boolean(true) }
	fn 'F'() { Value::Boolean(false) }
	fn 'N'() { Value::Null }
	fn 'R'() { Value::Number(rand::random()) }

	fn 'P'() {
		let mut buf = String::new();

		if let Err(err) = std::io::stdin().read_line(&mut buf) {
			panic!("unable to read line from stdin: {}", err);
		}

		Value::String(buf)
	}

	fn 'E' (arg) {
		arg.run().as_string().parse::<Ast>().unwrap().run()
	}

	fn 'B' (block) {
		block.clone().into()
	}

	fn 'C' (block) {
		block.run().run()
	}

	fn '`' (cmd) {
		let out = std::process::Command::new("sh")
			.arg("-c")
			.arg(&*cmd.run().as_string())
			.output()
			.expect("unable to execute `sh` command");

		String::from_utf8_lossy(&out.stdout).into_owned().into()
	}

	fn 'Q' (code) {
		std::process::exit(code.run().as_number() as i32);
	}

	fn '!' (code) {
		Value::Boolean(!code.run().as_boolean())
	}

	fn 'L' (string) {
		Value::Number(string.run().as_string().len() as _)
	}

	fn 'O' (msg) {
		let ret = msg.run();
		let text = ret.as_string();

		if let Some(stripped) = text.strip_suffix('\\') {
			print!("{}", stripped);
		} else {
			println!("{}", text);
		}

		ret
	}

	fn '+' (lhs, rhs) {
		lhs.run() + rhs.run()
	}

	fn '-' (lhs, rhs) {
		lhs.run() - rhs.run()
	}

	fn '*' (lhs, rhs) {
		lhs.run() * rhs.run()
	}

	fn '/' (lhs, rhs) {
		lhs.run() / rhs.run()
	}

	fn '%' (lhs, rhs) {
		lhs.run() % rhs.run()
	}

	fn '^' (lhs, rhs) {
		lhs.run().pow(rhs.run())
	}

	fn '?' (lhs, rhs) {
		(lhs.run() == rhs.run()).into()
	}

	fn '<' (lhs, rhs) {
		lhs.run().lth(&rhs.run()).into()
	}

	fn '>' (lhs, rhs) {
		lhs.run().gth(&rhs.run()).into()
	}


	fn '&' (lhs, rhs) {
		let lhs = lhs.run();

		if lhs.as_boolean() {
			rhs.run()
		} else {
			lhs
		}
	}

	fn '|' (lhs, rhs) {
		let lhs = lhs.run();

		if lhs.as_boolean() {
			lhs
		} else {
			rhs.run()
		}
	}

	fn ';' (lhs, rhs) {
		lhs.run();
		rhs.run()
	}

	fn '=' (arg, rhs) {
		let ident =
			if let Ast::Identifier(ref ident) = arg {
				ident.clone()
			} else {
				arg.run().as_string().into_owned()
			};

		let rhs = rhs.run();
		crate::env::set(ident, rhs.clone());
		rhs
	}

	fn 'W' (lhs, rhs) {
		let mut ret = Value::Null;

		while lhs.run().as_boolean() {
			ret = rhs.run();
		}

		ret
	}

	fn 'I' (cond, iftrue, iffalse) {
		if cond.run().as_boolean() {
			iftrue.run()
		} else {
			iffalse.run()
		}
	}

	fn 'G' (string, start, length) {
		string.run()
			.as_string()
			.chars()
			.skip(start.run().as_number() as usize)
			.take(length.run().as_number() as usize)
			.collect::<String>()
			.into()
	}

	fn 'S' (string, start, length, repl) {
		let mut s = string.run().as_string().into_owned();
		let start = start.run().as_number() as usize;
		s.replace_range(start..start + length.run().as_number() as usize, &repl.run().as_string());

		Value::String(s)
	}
}
