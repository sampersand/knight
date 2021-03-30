use crate::{Value, RuntimeError, RcString};
use std::collections::HashSet;
use std::fmt::{self, Debug, Display, Formatter};
use std::io::{self, Write, Read, BufReader};
use std::convert::TryFrom;
use std::hash::{Hash, Hasher};
use std::rc::Rc;

mod builder;
mod variable;

pub use builder::Builder;
pub use variable::Variable;

type RunCommand = dyn FnMut(&str) -> Result<RcString, RuntimeError>;

/// The set of [`Variable`]s within Knight.
///
/// # TODO: more details
/// ```rust
/// assert!(false);
/// ```
pub struct Environment<'i, 'o, 'c> {
	vars: HashSet<Variable>, // technically _not_ static, but no way to express `'self` in the type system.
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

impl Default for Environment<'_, '_, '_> {
	fn default() -> Self {
		Self::new()
	}
}

impl<'i, 'o, 'c> Environment<'i, 'o, 'c> {
	/// Creates an empty [`Environment`].
	///
	/// # Examples
	/// ```rust
	/// use knightrs::Environment;
	/// use std::io;
	///
	/// 
	/// ```
	pub fn new() -> Self {
		Self::builder().build()
	}
	// pub fn new(capacity: usize, stdin: &'i mut dyn Read, stdout: &'o mut dyn Write, run_command: &'c mut RunCommand) -> Self {
	// 	Self {
	// 		vars: Default::default(),
	// 		stdin,
	// 		stdout,
	// 		run_command
	// 	}
	// }

	pub fn builder() -> Builder<'i, 'o, 'c> {
		Builder::default()
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
		if let Some(inner) = self.vars.get(name.as_ref()) {
			return inner.clone();
		}

		let variable = Variable::_new(name.to_string().into_boxed_str());

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
