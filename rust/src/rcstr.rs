use std::rc::Rc;
use std::fmt::{self, Debug, Display, Formatter};
use std::hash::{Hash, Hasher};

#[derive(Clone)]
pub struct RcStr(Inner);

#[derive(Debug, Clone)]
enum Inner {
	Literal(&'static [u8]),
	Shared(Rc<str>)
}

impl Default for RcStr {
	fn default() -> Self {
		unsafe {
			RcStr::new_literal_unchecked(b"")
		}
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

#[derive(Debug, PartialEq, Eq)]
pub struct InvalidString(
	#[cfg(not(feature = "fatal-errors"))]
	InvalidStringInner,

	#[cfg(feature = "fatal-errors")]
	std::convert::Infallible
);


#[derive(Debug, PartialEq, Eq)]
#[cfg(not(feature = "reckless"))]
struct InvalidStringInner {
	byte: u8,
	idx: usize
}

#[cfg(not(feature="fatal-errors"))]
impl InvalidString {
	pub const fn byte(&self) -> u8 { self.0.byte }
	pub const fn idx(&self) -> usize { self.0.idx }
}

impl Display for InvalidString {
	fn fmt(&self, f: &mut Formatter) -> fmt::Result {
		cfg_if! {
			if #[cfg(feature = "fatal-errors")] {
				let _ = f;
				unsafe { unreachable_unchecked!() }
			} else {
				Display::fmt(&self.0, f)
			}
		}
	}
}

#[cfg(not(feature = "reckless"))]
impl Display for InvalidStringInner {
	fn fmt(&self, f: &mut Formatter) -> fmt::Result {
		write!(f, "invalid byte '\\x{:02x}' found at position {}", self.byte, self.idx)
	}
}

impl std::error::Error for InvalidString {}

fn validate_string(data: &[u8]) -> Result<(), InvalidString> {
	for (idx, &byte) in data.iter().enumerate() {
		if !matches!(byte, b'\r' | b'\n' | b' '..=b'~') {
			#[cfg(not(feature = "reckless"))]
			let inner = InvalidStringInner { byte, idx };

			cfg_if! {
				if #[cfg(feature = "reckless")] {
					unsafe { unreachable_unchecked!() }
				} else if #[cfg(feature = "fatal-errors")] {
					panic!("invalid string: {}", inner);
				} else {
					return Err(InvalidString(inner));
				}
			}
		}
	}

	Ok(())
}


impl RcStr {
	#[inline]
	pub fn new_literal(literal: &'static [u8]) -> Result<Self, InvalidString> {
		validate_string(literal)
			.map(|_| unsafe { Self::new_literal_unchecked(literal) })
	}

	pub unsafe fn new_literal_unchecked(literal: &'static [u8]) -> Self {
		debug_assert_eq!(validate_string(literal), Ok(()), "invalid literal encountered: {:?}", literal);

		Self(Inner::Literal(literal))
	}

	pub fn new_shared(string: impl ToString) -> Result<Self, InvalidString> {
		Ok(Self(Inner::Shared(string.to_string().into())))
	}

	pub fn as_str(&self) -> &str {
		match &self.0 {
			Inner::Literal(literal) => unsafe { std::str::from_utf8_unchecked(literal) },
			Inner::Shared(shared) => &*shared
		}
	}

	pub unsafe fn from_raw(raw: *const u8) -> Self {
		todo!()
	}

	pub fn into_raw(self) -> *const u8 {
		todo!()
	}
}

impl From<&'static str> for RcStr {
	#[inline]
	fn from(literal: &'static str) -> Self {
		Self::new_literal(literal.as_bytes()).unwrap()
	}
}

impl From<Rc<str>> for RcStr {
	#[inline]
	fn from(shared: Rc<str>) -> Self {
		Self(Inner::Shared(shared))
	}
}

impl std::convert::TryFrom<String> for RcStr {
	type Error = InvalidString;

	#[inline]
	fn try_from(string: String) -> Result<Self, InvalidString> {
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