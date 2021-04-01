use super::{RunCommand, Environment};
use crate::{RcString, RuntimeError};
use std::io::{self, Write, Read};
use std::collections::HashSet;
use std::fmt::{self, Display, Formatter};
use std::convert::TryFrom;

/// A Builder for the [`Environment`] struct.
#[derive(Default)]
pub struct Builder<'i, 'o, 'c> {
	capacity: Option<usize>,
	stdin: Option<&'i mut dyn Read>,
	stdout: Option<&'o mut dyn Write>,
	run_command: Option<&'c mut RunCommand>,
}

// We have a lot of private ZST structs here and `static mut`s. This is because we need to have a mutable reference to,
// eg, a `dyn read`, but `io::stdin()` will return a new object. Thus, we simply make a ZST that calls `io::stdin()`
// every time it needs to read something.

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
	fn fmt(&self, f: &mut Formatter<'_>) -> fmt::Result {
		write!(f, "cannot run '`' when as it is disabled.")
	}
}

impl std::error::Error for NotEnabled {}

fn run_command_err(_: &str) -> Result<RcString, RuntimeError> {
	Err(RuntimeError::Custom(Box::new(NotEnabled)))
}

fn run_command_system(cmd: &str) -> Result<RcString, RuntimeError> {
	let output =
		std::process::Command::new("sh")
			.arg("-c")
			.arg(cmd)
			.output()
			.map(|out| String::from_utf8_lossy(&out.stdout).into_owned())?;

	RcString::try_from(output).map_err(From::from)
}

static mut RUNCOMMAND_ERR: fn(&str) -> Result<RcString, RuntimeError> = run_command_err;
static mut RUNCOMMAND_SYSTEM: fn(&str) -> Result<RcString, RuntimeError> = run_command_system;

impl<'i, 'o, 'c> Builder<'i, 'o, 'c> {
	/// Creates a new, default [`Builder`].
	#[must_use = "creating a builder does nothing by itself."]
	pub fn new() -> Self {
		Self::default()
	}

	/// Sets the initial starting capacity for the set of [`Variable`](crate::Variable)s.
	///
	/// If not set, an (unspecified) default capacity is used.
	#[must_use = "assigning a capacity does nothing without calling 'build'."]
	pub fn capacity(mut self, capacity: usize) -> Self {
		self.capacity = Some(capacity);
		self
	}

	/// Sets the stdin for the [`Environment`].
	///
	/// This defaults to the [stdin](io::stdin) of the process.
	#[must_use = "assigning to stdin does nothing without calling 'build'."]
	pub fn stdin(mut self, stdin: &'i mut dyn Read) -> Self {
		self.stdin = Some(stdin);
		self
	}

	/// Sets the stdout for the [`Environment`].
	///
	/// This defaults to the [stdout](io::stdout) of the process.
	#[must_use = "assigning to stdout does nothing without calling 'build'."]
	pub fn stdout(mut self, stdout: &'o mut dyn Write) -> Self {
		self.stdout = Some(stdout);
		self
	}

	/// Explicitly sets what should happen when the ["system" (`` ` ``)](crate::function::system) function is called.
	///
	/// The default value is to simply send the command to `sh` (ie `"sh", "-c", "command"`)
	#[must_use = "assigning a 'run_command' does nothing without calling 'build'."]
	pub fn run_command(mut self, run_command: &'c mut RunCommand) -> Self {
		self.run_command = Some(run_command);
		self
	}

	/// Disables the ["system" (`` ` ``)](crate::function::system) command entirely.
	///
	/// When this is enabled, all calls to [`` ` ``](crate::function::system) will return errors.
	#[must_use = "disabling the system command to does nothing without calling 'build'."]
	pub fn disable_system(self) -> Self {
		// SAFETY: We're getting a mutable reference to a ZST, so this is always safe.
		self.run_command(unsafe { &mut RUNCOMMAND_ERR })
	}

	/// Creates a new [`Environment`] with all the supplied options.
	///
	/// Any options that have not been explicitly set will have their default values used.
	#[must_use = "Simply calling `build` does nothing on its own."]
	pub fn build(self) -> Environment<'i, 'o, 'c> {
		// SAFETY: All of these `unsafe` blocks are simply mutable references to ZSTs, which is always safe.
		Environment {
			vars: HashSet::with_capacity(self.capacity.unwrap_or(2048)),
			stdin: self.stdin.unwrap_or(unsafe { &mut STDIN }),
			stdout: self.stdout.unwrap_or(unsafe { &mut STDOUT }),
			run_command: self.run_command.unwrap_or(unsafe { &mut RUNCOMMAND_SYSTEM })
		}
	}
}
