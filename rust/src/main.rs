use knight::{RuntimeError, Environment};
use clap::{App, Arg, ArgMatches};

fn run(matches: ArgMatches) -> Result<(), RuntimeError> {
	let env = Environment::new();

	if let Some(expr) = matches.value_of("expr") {
		knight::run_str(&expr, &env)?;
	} else if let Some(filename) = matches.value_of("file") {
		knight::run_str(std::fs::read_to_string(filename)?, &env)?;
	} else {
		eprintln!("{}", matches.usage());
		std::process::exit(1);
	}

	Ok(())
}

fn main() {
	let matches =
		App::new("knight")
			.version(clap::crate_version!())
			.author(clap::crate_authors!())
			.about("The Knight programming language")
			.usage("knight (-e 'expr' | -f file)")
			.arg(Arg::with_name("expr")
				.help("the expression to execute")
				.takes_value(true)
				.conflicts_with("file")
				.short("e")
				.long("expr"))
			.arg(Arg::with_name("file")
				.help("the expression to read and execute")
				.takes_value(true)
				.conflicts_with("expr")
				.short("f")
				.long("file"))
		.get_matches();

	if let Err(err) = run(matches) {
		eprintln!("error: {}", err);
		std::process::exit(1);
	}
}