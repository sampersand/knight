use crate::Value;
use std::collections::HashSet;
use std::hash::{Hash, Hasher};

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
		use std::rc::Rc;

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
#[derive(Debug, Default)]
pub struct Environment(Lock<HashSet<Variable>>);

/// A variable within Knight.
///
/// All variables of with the same [`name`] from the same [`Environment`] are identical to one another: updating one
/// (via [`assign`]) will update all the others.
#[derive(Debug, Clone)]
pub struct Variable(Rc<VariableInner>);

#[derive(Debug)]
struct VariableInner {
	name: Box<str>,
	value: Lock<Option<Value>>
}

impl std::borrow::Borrow<str> for Variable {
	fn borrow(&self) -> &str {
		&self.0.name
	}
}

impl Hash for Variable {
	fn hash<H: Hasher>(&self, h: &mut H) {
		self.0.name.hash(h);
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

impl Environment {
	/// Creates an empty [`Environment`].
	pub fn new() -> Self {
		Self::default()
	}

	/// Creates an [`Environment`] with the given starting capacity..
	pub fn with_capacity(capacity: usize) -> Self {
		Self(Lock::new(HashSet::with_capacity(capacity)))
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
		if let Some(variable) = access!(read; self.0).get(name.as_ref()) {
			return variable.clone();
		}

		let variable = Variable(Rc::new(VariableInner {
			name: name.to_string().into_boxed_str(),
			value: Default::default()
		}));

		access!(write; self.0).insert(variable.clone());

		variable
	}
}

impl Variable {
	/// Fetches the name of the variable.
	///
	/// # Examples
	/// ```rust
	/// # use knight::Environment;
	/// let env = Environment::new();
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
	/// let env = Environment::new();
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
	/// let env = Environment::new();
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
	/// let env = Environment::new();
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