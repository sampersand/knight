use crate::{Variable, RuntimeError, RcString};
use std::collections::HashSet;
use std::fmt::{self, Debug, Formatter};
use std::io::{self, Write, Read, BufReader};
use std::convert::TryFrom;


type RunCommand =  dyn FnMut(&str) -> Result<RcString, RuntimeError>;

/// The set of [`Variable`]s within Knight.
///
/// # TODO: more details
/// ```rust
/// assert!(false);
/// ```
pub struct Environment<'i, 'o, 'c> {
	vars: HashSet<Variable>,
	stdin: &'i mut dyn Read,
	stdout: &'o mut dyn Write,
	run_command: &'c mut RunCommand
}

impl Debug for Environment<'_, '_, '_> {
	fn fmt(&self, f: &mut Formatter) -> fmt::Result {
		f.debug_struct("Environment")
			.field("nvars", &self.vars.len())
			.finish()
	}
}

impl Default for Environment<'static, 'static, 'static> {
	fn default() -> Self {
		#[cfg(feature = "embedded")]
		fn run_command(_: &str) -> Result<RcString, RuntimeError> {
			Err(RuntimeError::Custom(Box::new("cannot run command when embedded.")))
		}

		#[cfg(not(feature = "embedded"))]
		fn run_command(cmd: &str) -> Result<RcString, RuntimeError> {
			RcString::try_from(
				std::process::Command::new("sh")
					.arg("-c")
					.arg(cmd)
					.output()
					.map(|out| String::from_utf8_lossy(&out.stdout).into_owned())?
			).map_err(From::from)
		}

		struct Stdin;
		struct Stdout;

		impl Read for Stdin {
			#[inline]
			fn read(&mut self, data: &mut [u8]) -> io::Result<usize> {
				io::stdin().read(data)
			}
		}

		impl Write for Stdout {
			#[inline]
			fn write(&mut self, data: &[u8]) -> io::Result<usize> {
				io::stdout().write(data)
			}

			#[inline]
			fn flush(&mut self) -> io::Result<()> {
				io::stdout().flush()
			}
		}

		static mut STDIN: Stdin = Stdin;
		static mut STDOUT: Stdout = Stdout;
		static mut RUNCOMMAND: fn(&str) -> Result<RcString, RuntimeError> = run_command;

		Self::new(unsafe { &mut STDIN }, unsafe { &mut STDOUT }, unsafe { &mut RUNCOMMAND })
	}
}

impl<'i, 'o, 'c> Environment<'i, 'o, 'c> {
	/// Creates an empty [`Environment`].
	pub fn new(stdin: &'i mut dyn Read, stdout: &'o mut dyn Write, run_command: &'c mut RunCommand) -> Self {
		Self {
			vars: Default::default(),
			stdin,
			stdout,
			run_command
		}
	}

	/// Creates an [`Environment`] with the given starting capacity..
	pub fn with_capacity(
		capacity: usize,
		stdin: &'i mut dyn Read,
		stdout: &'o mut dyn Write,
		run_command: &'c mut RunCommand
	) -> Self {
		Self {
			vars: HashSet::with_capacity(capacity),
			stdin,
			stdout,
			run_command
		}
	}

	/// Retrieves the variable with the given name.
	///
	/// If a variable with `name` already exists in the environment, it is returned. Otherwise, a new, unassigned
	/// variable is created and then returned.
	///
	/// # Examples
	/// ```rust
	/// # use knight::Environment;
	/// let mut env = Environment::new();
	/// let var = env.get("plato");
	///
	/// assert_eq!(var, env.get("plato"));
	/// ```
	pub fn get<N: AsRef<str> + ToString>(&mut self, name: N) -> Variable {
		if let Some(variable) = self.vars.get(name.as_ref()) {
			return variable.clone();
		}

		let variable = Variable::create(name.to_string().into_boxed_str());

		self.vars.insert(variable.clone());

		variable
	}

	pub fn run_command(&mut self, cmd: &str) -> Result<RcString, RuntimeError> {
		(self.run_command)(cmd)
	}
}

impl Read for Environment<'_, '_, '_> {
	#[inline]
	fn read(&mut self, data: &mut [u8]) -> io::Result<usize> {
		self.stdin.read(data)
	}
}

impl Write for Environment<'_, '_, '_> {
	#[inline]
	fn write(&mut self, data: &[u8]) -> io::Result<usize> {
		self.stdout.write(data)
	}

	#[inline]
	fn flush(&mut self) -> io::Result<()> {
		self.stdout.flush()
	}
}