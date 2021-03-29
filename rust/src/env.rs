use crate::Variable;
use std::collections::HashSet;
use std::fmt::{self, Debug, Formatter};
use std::io::{self, Read, Write, BufRead, BufReader};

/// The set of [`Variable`]s within Knight.
///
/// # TODO: more details
/// ```rust
/// assert!(false);
/// ```
pub struct Environment {
	vars: HashSet<Variable>,
	stdin: Box<dyn BufRead>,
	stdout: Box<dyn Write>
}

impl Debug for Environment {
	fn fmt(&self, f: &mut Formatter) -> fmt::Result {
		f.debug_struct("Environment")
			.field("nvars", &self.vars.len())
			.finish()
	}
}

impl Default for Environment {
	fn default() -> Self {
		Self::new(BufReader::new(io::stdin()), io::stdout())
	}
}

impl Environment {
	/// Creates an empty [`Environment`].
	pub fn new<I, O>(stdin: I, stdout: O) -> Self
	where
		I: BufRead + 'static,
		O: Write + 'static
	{
		Self {
			vars: Default::default(),
			stdin: Box::new(stdin),
			stdout: Box::new(stdout)
		}
	}

	/// Creates an [`Environment`] with the given starting capacity..
	pub fn with_capacity<I, O>(capacity: usize, stdin: I, stdout: O) -> Self
	where
		I: BufRead + 'static,
		O: Write + 'static
	{
		Self {
			vars: HashSet::with_capacity(capacity),
			stdin: Box::new(stdin),
			stdout: Box::new(stdout),
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
}

impl Read for Environment {
	#[inline]
	fn read(&mut self, data: &mut [u8]) -> io::Result<usize> {
		self.stdin.read(data)
	}
}

impl BufRead for Environment {
	#[inline]
	fn fill_buf(&mut self) -> io::Result<&[u8]> {
		self.stdin.fill_buf()
	}

	#[inline]
	fn consume(&mut self, amnt: usize) {
		self.stdin.consume(amnt)
	}
}

impl Write for Environment {
	#[inline]
	fn write(&mut self, data: &[u8]) -> io::Result<usize> {
		self.stdout.write(data)
	}

	#[inline]
	fn flush(&mut self) -> io::Result<()> {
		self.stdout.flush()
	}
}