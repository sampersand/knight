use std::rc::Rc;
use std::fmt::{self, Debug, Display, Formatter};
use std::hash::{Hash, Hasher};

#[derive(Clone)]
pub struct RcStr(Inner);

#[derive(Debug, Clone)]
enum Inner {
	Literal(&'static str),
	Shared(Rc<str>)
}

impl Default for RcStr {
	fn default() -> Self {
		RcStr::new_literal("")
	}
}

impl Debug for RcStr {
	fn fmt(&self, f: &mut Formatter) -> fmt::Result {
		if !f.alternate() {
			return Debug::fmt(self.as_str(), f);
		}

		match &self.0 {
			Inner::Literal(literal) =>
				f.debug_tuple("RcStr::Literal")
					.field(&literal)
					.finish(),

			Inner::Shared(shared) =>
				f.debug_tuple("RcStr::Shared")
					.field(&shared)
					.finish()
		}
	}
}

impl Display for RcStr {
	fn fmt(&self, f: &mut Formatter) -> fmt::Result {
		Display::fmt(self.as_str(), f)
	}
}

impl Hash for RcStr {
	fn hash<H: Hasher>(&self, h: &mut H) {
		self.as_str().hash(h)
	}
}

impl Eq for RcStr {}
impl PartialEq for RcStr {
	fn eq(&self, rhs: &Self) -> bool {
		self.as_str() == rhs.as_str()
	}
}

impl PartialOrd for RcStr {
	fn partial_cmp(&self, rhs: &Self) -> Option<std::cmp::Ordering> {
		self.as_str().partial_cmp(rhs.as_str())
	}
}

impl Ord for RcStr {
	fn cmp(&self, rhs: &Self) -> std::cmp::Ordering {
		self.as_str().cmp(rhs.as_str())
	}
}

impl RcStr {
	#[inline]
	pub const fn new_literal(literal: &'static str) -> Self {
		Self(Inner::Literal(literal))
	}

	pub fn new_shared(string: impl ToString) -> Self {
		Self(Inner::Shared(string.to_string().into()))
	}

	pub fn as_str(&self) -> &str {
		match &self.0 {
			Inner::Literal(literal) => literal,
			Inner::Shared(shared) => &*shared
		}
	}
}

impl From<&'static str> for RcStr {
	#[inline]
	fn from(literal: &'static str) -> Self {
		Self::new_literal(literal)
	}
}

impl From<Rc<str>> for RcStr {
	#[inline]
	fn from(shared: Rc<str>) -> Self {
		Self(Inner::Shared(shared))
	}
}

impl From<String> for RcStr {
	#[inline]
	fn from(string: String) -> Self {
		Self::new_shared(string)
	}
}

impl AsRef<str> for RcStr {
	#[inline]
	fn as_ref(&self) -> &str {
		self.as_str()
	}
}

impl std::ops::Deref for RcStr {
	type Target = str;

	fn deref(&self) -> &Self::Target {
		self.as_str()
	}
}