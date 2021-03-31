use knightrs::{RuntimeError, Environment};
use clap::{App, Arg, ArgMatches};

fn run(matches: ArgMatches<'_>) -> Result<(), RuntimeError> {
	let mut env = Environment::default();

	if let Some(expr) = matches.value_of("expr") {
		knightrs::run_str(&expr, &mut env)?;
	} else if let Some(filename) = matches.value_of("file") {
		knightrs::run_str(std::fs::read_to_string(filename)?, &mut env)?;
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
		// .get_matches_from(vec!["--", "-f", "../knight.kn"]);
		.get_matches();

	match run(matches) {
		Err(RuntimeError::Quit(code)) => std::process::exit(code),
		Err(err) => {
			eprintln!("error: {}", err);
			std::process::exit(1)
		},
		Ok(()) => { /* do nothing */ }
	}
}
