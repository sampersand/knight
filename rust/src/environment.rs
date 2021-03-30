use crate::{Value, RuntimeError, RcString};
use std::collections::HashSet;
use std::fmt::{self, Debug, Formatter};
use std::io::{self, Write, Read, BufReader};
use std::convert::TryFrom;
use std::hash::{Hash, Hasher};
use std::rc::Rc;

type RunCommand =  dyn FnMut(&str) -> Result<RcString, RuntimeError>;

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

/// A variable within Knight.
///
/// All variables of with the same [`name`] from the same [`Environment`] are identical to one another: updating one
/// (via [`assign`]) will update all the others.
#[derive(Clone)]
pub struct Variable(Rc<VariableInner>);

struct VariableInner {
	name: Box<str>,
	value: Lock<Option<Value>>
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
			fn read(&mut self, data: &mut [u8]) -> io::Result<usize> { io::stdin().read(data) }
		}

		impl Write for Stdout {
			#[inline]
			fn write(&mut self, data: &[u8]) -> io::Result<usize> { io::stdout().write(data) }

			#[inline]
			fn flush(&mut self) -> io::Result<()> { io::stdout().flush() }
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
		if let Some(inner) = self.vars.get(name.as_ref()) {
			return inner.clone();
		}

		let variable =
			Variable(Rc::new(VariableInner {
				name: name.to_string().into_boxed_str(),
				value: Default::default()
			}));

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

cfg_if! {
	if #[cfg(feature = "multithreaded")] {
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

impl std::borrow::Borrow<str> for Variable {
	fn borrow(&self) -> &str {
		self.name()
	}
}

impl Hash for Variable {
	fn hash<H: Hasher>(&self, h: &mut H) {
		self.name().hash(h);
	}
}

impl Eq for Variable {}
impl PartialEq for Variable {
	/// Checks to see if two variables are the same.
	///
	/// This will only return `true` if they both originate from the same [`Environment`] and have the same [`name`](
	/// Variable::name).
	fn eq(&self, rhs: &Self) -> bool {
		Rc::ptr_eq(&self.0, &rhs.0)
	}
}

impl Debug for Variable {
	fn fmt(&self, f: &mut Formatter) -> fmt::Result {
		if f.alternate() {
			f.debug_struct("Variable")
				.field("name", &self.0.name)
				.field("value", &access!(read; self.0.value))
				.finish()
		} else {
			f.debug_tuple("Variable")
				.field(&self.0.name)
				.finish()
		}
	}
}

impl Variable {
	/// Fetches the name of the variable.
	///
	/// # Examples
	/// ```rust
	/// # use knight::Environment;
	/// let mut env = Environment::default();
	/// let var = env.get("plato");
	///
	/// assert_eq!(var.name(), "plato");
	/// ```
	pub fn name(&self) -> &str {
		&self.0.name
	}

	/// Checks to see if the variable has been [`assign`]ed to yet.
	///
	/// # Examples
	/// ```rust
	/// # use knight::{Environment, Value};
	/// let mut env = Environment::default();
	/// let var = env.get("plato");
	///
	/// assert!(!var.is_assigned());
	/// 
	/// var.assign(Value::from(true));
	/// assert!(var.is_assigned());
	/// ```
	pub fn is_assigned(&self) -> bool {
		access!(read; self.0.value).is_some()
	}

	/// Associates `value` with this variable, so that [`fetch`] will return it.
	/// 
	/// Any previously associated [`Value`]s are discarded.
	///
	/// Note that all variables with the same name from the same environment will be affected. After all, that's the
	/// point of having variables.
	///
	/// # Examples
	/// ```rust
	/// # use knight::{Environment, Value};
	/// let mut env = Environment::default();
	/// let var = env.get("plato");
	/// let var2 = env.get("plato");
	/// 
	/// var.assign(Value::from(true));
	/// assert_eq!(var.fetch().unwrap(), Value::from(true));
	/// assert_eq!(var2.fetch().unwrap(), Value::from(true));
	/// ```
	pub fn assign(&self, value: Value) {
		access!(write; self.0.value).replace(value);
	}

	/// Returns the last value associated with this variable, or `None` if nothing's been associated.
	///
	/// Note that all variables with the same name from the same environment will be affected. After all, that's the
	/// point of having variables.
	///
	/// # Examples
	/// ```rust
	/// # use knight::{Environment, Value};
	/// let mut env = Environment::default();
	/// let var = env.get("plato");
	/// 
	/// assert_eq!(var.fetch(), None);
	/// var.assign(Value::from(true));
	/// assert_eq!(var.fetch(), Some(Value::from(true)));
	/// ```
	pub fn fetch(&self) -> Option<Value> {
		access!(read; self.0.value).clone()
	}
}