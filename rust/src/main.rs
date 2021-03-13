use knight::RuntimeError;
use clap::{App, Arg, ArgMatches};

fn run(matches: ArgMatches) -> Result<(), RuntimeError> {
	if let Some(expr) = matches.value_of("expr") {
		knight::run_str(&expr)?;
	} else {
		let filename = matches.value_of("file").unwrap();
		knight::run_str(std::fs::read_to_string(filename)?)?;
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
		// .get_matches_from(vec![ "knight", "-e", "D + 9 '1a"]);

	if let Err(err) = run(matches) {
		eprintln!("error: {}", err);
		std::process::exit(1);
	}
}