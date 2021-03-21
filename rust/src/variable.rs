use crate::{Value, RuntimeError};
use std::collections::HashMap;
use once_cell::unsync::OnceCell;
use std::cell::RefCell;
use std::fmt::{self, Debug, Formatter};
use std::hash::{Hash, Hasher};

#[derive(Clone, Copy)]
#[repr(transparent)]
pub struct Variable(&'static Inner);

struct Inner {
	name: &'static str,
	value: RefCell<Option<Value>>
}

impl Variable {
	pub fn new(name: &str) -> Self {
		Self::fetch_or_leak(name, |name| Box::leak(name.to_string().into_boxed_str()))
	}

	pub fn fetch_or_leak<N: AsRef<str>>(name: N, leak: impl FnOnce(N) -> &'static str) -> Self {
		static mut ENVIRONMENT: OnceCell<RefCell<HashMap<&'static str, &'static Inner>>> = OnceCell::new();

		let mut env = unsafe { ENVIRONMENT.get_or_init(Default::default) }.borrow_mut();

		if let Some(variable) = env.get(name.as_ref()) {
			return Self(variable);
		}

		let name = leak(name);
		let variable = Box::leak(Box::new(Inner { name, value: Default::default() }));
		env.insert(name, variable);

		Self(variable)
	}

	pub const fn name(&self) -> &'static str {
		self.0.name
	}

	pub fn assign(&self, value: Value) {
		*self.0.value.borrow_mut() = Some(value);
	}

	pub fn run(&self) -> Result<Value, RuntimeError> {
		self.0.value.borrow()
			.as_ref()
			.ok_or_else(|| RuntimeError::UnknownIdentifier { identifier: self.name() })?
			.run()
	}
}

impl From<String> for Variable {
	#[inline]
	fn from(name: String) -> Self {
		Self::fetch_or_leak(name, |name| Box::leak(name.into_boxed_str()))
	}
}

impl From<&'static str> for Variable {
	#[inline]
	fn from(name: &'static str) -> Self {
		Self::fetch_or_leak(name, |name| name)
	}
}

impl Debug for Variable {
	fn fmt(&self, f: &mut Formatter) -> fmt::Result {
		if f.alternate() {
			f.debug_struct("Variable")
				.field("name", &self.name())
				.field("value", &self.0.value.borrow())
				.finish()
		} else {
			f.debug_tuple("Variable")
				.field(&self.name())
				.finish()
		}
	}
}

impl Eq for Variable {}
impl PartialEq for Variable {
	fn eq(&self, rhs: &Self) -> bool {
		// check to see if pointers are equal

		(self.0 as *const _) == (rhs.0 as *const _)
	}
}

impl Hash for Variable {
	fn hash<H: Hasher>(&self, h: &mut H) {
		self.0.name.hash(h)
	}
}
