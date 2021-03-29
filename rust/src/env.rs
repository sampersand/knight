use crate::Variable;
use std::collections::HashSet;
use std::fmt::{self, Debug, Formatter};
use std::io::{self, Read, Write, BufRead, BufReader};

cfg_if! {
	if #[cfg(feature = "multithreaded")] {
		use std::sync::Arc as Rc;
		use parking_lot::RwLock;

		type Lock<T> = RwLock<T>;
		macro_rules! access {
			(read; $what:expr) => { $what.read() };
			(write; $what:expr) => { $what.write() };
		}
	} else {
		use std::cell::RefCell;
		

		type Lock<T> = RefCell<T>;
		macro_rules! access {
			(read; $what:expr) => { $what.borrow() };
			(write; $what:expr) => { $what.borrow_mut() };
		}
	}
}

/// The set of [`Variable`]s within Knight.
///
/// # TODO: more details
/// ```rust
/// assert!(false);
/// ```
pub struct Environment {
	vars: Lock<HashSet<Variable>>,
	stdin: Lock<Box<dyn BufRead>>,
	stdout: Lock<Box<dyn Write>>
}

impl Debug for Environment {
	fn fmt(&self, f: &mut Formatter) -> fmt::Result {
		f.debug_struct("Environment").field("nvars", &access!(read; self.vars).len()).finish()
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
			stdin: Lock::new(Box::new(stdin)),
			stdout: Lock::new(Box::new(stdout))
		}
	}

	/// Creates an [`Environment`] with the given starting capacity..
	pub fn with_capacity<I, O>(capacity: usize, stdin: I, stdout: O) -> Self
	where
		I: BufRead + 'static,
		O: Write + 'static
	{
		Self {
			vars: Lock::new(HashSet::with_capacity(capacity)),
			stdin: Lock::new(Box::new(stdin)),
			stdout: Lock::new(Box::new(stdout)),
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
	/// let env = Environment::new();
	/// let var = env.get("plato");
	///
	/// assert_eq!(var, env.get("plato"));
	/// ```
	pub fn get<N: AsRef<str> + ToString>(&self, name: N) -> Variable {
		if let Some(variable) = access!(read; self.vars).get(name.as_ref()) {
			return variable.clone();
		}

		let variable = Variable::create(name.to_string().into_boxed_str());

		access!(write; self.vars).insert(variable.clone());

		variable
	}


}

impl Read for Environment {
	fn read(&mut self, data: &mut [u8]) -> io::Result<usize> {
		access!(write; self.stdin).read(data)
	}
}

impl BufRead for Environment {
	fn fill_buf(&mut self) -> io::Result<&[u8]> {
		access!(write; self.stdin).fill_buf()
	}

	fn consume(&mut self, amnt: usize) {
		access!(write; self.stdin).consume(amnt)
	}
}

impl Write for Environment {
	fn write(&mut self, data: &[u8]) -> io::Result<usize> {
		access!(write; self.stdout).write(data)
	}

	fn flush(&mut self) -> io::Result<()> {
		access!(write; self.stdout).flush()
	}
}