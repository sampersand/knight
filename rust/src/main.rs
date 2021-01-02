macro_rules! die {
	($($tt:tt)*) => {{
		eprintln!($($tt)*);
		std::process::exit(1);
	}};
}
pub mod value;
pub mod ast;
pub mod function;
pub mod env;

use ast::Ast;
use value::Value;
use function::Function;

fn usage(prog: &str) -> ! {
	die!("usage: {} [-e 'program'] [-f file]", prog);
}

fn main() {
	let mut args = std::env::args();
	let prog = args.next().unwrap();


	let cmd = args.next().unwrap_or_else(|| usage(&prog));
	let arg = args.next().unwrap_or_else(|| usage(&prog));

	let input =
		match cmd.as_str() {
			"-e" => arg,
			"-f" => std::fs::read_to_string(&arg).expect("Can't read file!"),
			_ => usage(&prog)
		};

	env::initialize();

	input.parse::<Ast>().unwrap().run();
}
