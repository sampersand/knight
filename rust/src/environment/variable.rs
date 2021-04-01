use crate::Value;
use std::fmt::{self, Debug, Formatter};
use std::hash::{Hash, Hasher};
use std::rc::Rc;
use std::cell::RefCell;

/// A variable within Knight.
///
/// Note that there is no way to create [`Variable`]s directly---they must to be fetched via [`Environment::get`](
/// crate::Environment::get).
///
/// All variables with the same from the same [`Environment`](crate::Environment) are identical. Updating any one of
/// them (via [`Variable::assign`]) will update them all.
#[derive(Clone)]
pub struct Variable(Rc<Inner>);

struct Inner {
	name: Box<str>,
	value: RefCell<Option<Value>>
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
	/// This will only return `true` if they both originate from the same [`Environment`](crate::Environment) and have
	/// the same [`name`](Self::name).
	fn eq(&self, rhs: &Self) -> bool {
		Rc::ptr_eq(&self.0, &rhs.0)
	}
}

impl Debug for Variable {
	fn fmt(&self, f: &mut Formatter<'_>) -> fmt::Result {
		if f.alternate() {
			f.debug_struct("Variable")
				.field("name", &self.0.name)
				.field("value", &self.0.value.borrow())
				.finish()
		} else {
			f.debug_tuple("Variable")
				.field(&self.0.name)
				.finish()
		}
	}
}

impl Variable {
	pub(super) fn _new(name: Box<str>) -> Self {
		Self(Rc::new(Inner { name, value: RefCell::default() }))
	}

	/// Fetches the name of the variable.
	///
	/// # Examples
	/// ```rust
	/// # use knightrs::Environment;
	/// let mut env = Environment::default();
	/// let var = env.get("plato");
	///
	/// assert_eq!(var.name(), "plato");
	/// ```
	#[must_use]
	pub fn name(&self) -> &str {
		&self.0.name
	}

	/// Checks to see if the variable has been [`assign`](Self::assign)ed to yet.
	///
	/// # Examples
	/// ```rust
	/// # use knightrs::{Environment, Value};
	/// let mut env = Environment::default();
	/// let var = env.get("plato");
	///
	/// assert!(!var.is_assigned());
	/// 
	/// var.assign(Value::Null);
	/// assert!(var.is_assigned());
	/// ```
	#[must_use = "this simply checks to see if it's assigned, and doesn't do anthing on its own."]
	pub fn is_assigned(&self) -> bool {
		self.0.value.borrow().is_some()
	}

	/// Associates `value` with this variable, so that [`fetch`](Self::fetch) will return it.
	/// 
	/// Any previously associated [`Value`]s are discarded.
	///
	/// Note that all variables with the same name from the same environment will be affected. After all, that's the
	/// point of having variables.
	///
	/// # Examples
	/// ```rust
	/// # use knightrs::{Environment, Value};
	/// let mut env = Environment::default();
	/// let var = env.get("plato");
	/// let var2 = env.get("plato");
	/// 
	/// var.assign(Value::Null);
	/// assert_eq!(var.fetch().unwrap(), Value::Null);
	/// assert_eq!(var2.fetch().unwrap(), Value::Null);
	/// ```
	pub fn assign(&self, value: Value) {
		self.0.value.borrow_mut().replace(value);
	}

	/// Returns the last value associated with this variable, or `None` if nothing's been associated.
	///
	/// Note that all variables with the same name from the same environment will be affected. After all, that's the
	/// point of having variables.
	///
	/// # Examples
	/// ```rust
	/// # use knightrs::{Environment, Value};
	/// let mut env = Environment::default();
	/// let var = env.get("plato");
	/// 
	/// assert_eq!(var.fetch(), None);
	/// var.assign(Value::from(true));
	/// assert_eq!(var.fetch(), Some(Value::from(true)));
	/// ```
	#[must_use = "simply fetching a value does nothing; the return value should be inspected."]
	pub fn fetch(&self) -> Option<Value> {
		self.0.value.borrow().clone()
	}
}