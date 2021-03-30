use crate::{RcString, RuntimeError};
use std::io::{self, Write, Read};
use super::{RunCommand, Environment};
use std::collections::HashSet;
use std::fmt::{self, Display, Formatter};
use std::convert::TryFrom;

#[derive(Default)]
pub struct Builder<'i, 'o, 'c> {
	capacity: Option<usize>,
	embedded: bool,
	stdin: Option<&'i mut dyn Read>,
	stdout: Option<&'o mut dyn Write>,
	run_command: Option<&'c mut RunCommand>,
}

struct Stdin;
static mut STDIN: Stdin = Stdin;

impl Read for Stdin {
	fn read(&mut self, data: &mut [u8]) -> io::Result<usize> {
		io::stdin().read(data)
	}
}

struct Stdout;
static mut STDOUT: Stdout = Stdout;

impl Write for Stdout {
	fn write(&mut self, data: &[u8]) -> io::Result<usize> {
		io::stdout().write(data)
	}

	fn flush(&mut self) -> io::Result<()> {
		io::stdout().flush()
	}
}

#[derive(Debug)]
struct NotEnabled;

impl Display for NotEnabled {
	fn fmt(&self, f: &mut Formatter) -> fmt::Result {
		write!(f, "cannot run ` when embedded.")
	}
}

impl std::error::Error for NotEnabled {}

fn run_command_err(_: &str) -> Result<RcString, RuntimeError> {
	Err(RuntimeError::Custom(Box::new(NotEnabled)))
}

fn run_command_system(cmd: &str) -> Result<RcString, RuntimeError> {
	RcString::try_from(
		std::process::Command::new("sh")
			.arg("-c")
			.arg(cmd)
			.output()
			.map(|out| String::from_utf8_lossy(&out.stdout).into_owned())?
	).map_err(From::from)
}

static mut RUNCOMMAND_ERR: fn(&str) -> Result<RcString, RuntimeError> = run_command_err;
static mut RUNCOMMAND_SYSTEM: fn(&str) -> Result<RcString, RuntimeError> = run_command_system;


impl<'i, 'o, 'c> Builder<'i, 'o, 'c> {
	pub fn new() -> Self {
		Self::default()
	}

	pub fn capacity(mut self, capacity: usize) -> Self {
		self.capacity = Some(capacity);
		self
	}

	pub fn stdin(mut self, stdin: &'i mut dyn Read) -> Self {
		self.stdin = Some(stdin);
		self
	}

	pub fn run_command(mut self, run_command: &'c mut RunCommand) -> Self {
		self.run_command = Some(run_command);
		self
	}

	pub fn embedded(mut self, embedded: bool) -> Self {
		self.embedded = embedded;
		self
	}

	pub fn build(self) -> Environment<'i, 'o, 'c> {
		Environment {
			vars: HashSet::with_capacity(self.capacity.unwrap_or(0)),
			stdin: self.stdin.unwrap_or(unsafe { &mut STDIN }),
			stdout: self.stdout.unwrap_or(unsafe { &mut STDOUT }),
			run_command: self.run_command.unwrap_or(unsafe {
				if self.embedded { &mut RUNCOMMAND_ERR} else { &mut RUNCOMMAND_SYSTEM }
			})
		}
	}
}