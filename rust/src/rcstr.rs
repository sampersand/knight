//! Types relating to the [`RcStr`].

use std::sync::Arc;
use std::fmt::{self, Debug, Display, Formatter};
use std::hash::{Hash, Hasher};

/// The string type within Knight.
#[derive(Clone)]
pub struct RcStr(Arc<str>);

impl Default for RcStr {
	fn default() -> Self {
		use once_cell::sync::OnceCell;

		static EMPTY: OnceCell<RcStr> = OnceCell::new();

		EMPTY.get_or_init(|| Self(Arc::from(""))).clone()
	}
}

impl Debug for RcStr {
	fn fmt(&self, f: &mut Formatter) -> fmt::Result {
		Debug::fmt(self.as_str(), f)
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

/// An error that indicates a character within a Knight string wasn't valid.
#[derive(Debug, PartialEq, Eq)]
pub struct InvalidChar {
	/// The byte that was invalid.
	pub chr: char,

	/// The index of the invalid byte in the given string.
	pub idx: usize
}

impl Display for InvalidChar {
	fn fmt(&self, f: &mut Formatter) -> fmt::Result {
		write!(f, "invalid byte '{:?}' found at position {}", self.chr, self.idx)
	}
}

impl std::error::Error for InvalidChar {}

/// Checks to see if `chr` is a valid knight character.
pub fn is_valid_char(chr: char) -> bool {
	return matches!(chr, '\r' | '\n' | ' '..='~');
}

fn validate_string(data: &str) -> Result<(), InvalidChar> {
	for (idx, chr) in data.chars().enumerate() {
		if !is_valid_char(chr) {
			return Err(InvalidChar { chr, idx });
		}
	}

	Ok(())
}

impl RcStr {
	/// Creates a new `RcStr` with the given input string.
	///
	/// # Errors
	/// If `string` contains any characters which aren't valid in Knight source code, an `InvalidChar` is returned.
	///
	/// # See Also
	/// - [`RcStr::new_unchecked`] For a version which doesn't verify `string`.
	pub fn new<T: AsRef<str> + Into<Arc<str>>>(string: T) -> Result<Self, InvalidChar> {
		validate_string(string.as_ref())?;

		// SAFETY: we just validated the string.
		unsafe {
			Ok(Self::new_unchecked(string))
		}
	}

	/// Creates a new `RcStr`, without verifying that the string is valid.
	///
	/// # Safety
	/// All characters within the string must be valid for Knight strings. See the specs for what exactly this entails.
	pub unsafe fn new_unchecked<T: AsRef<str> + Into<Arc<str>>>(string: T) -> Self {
		debug_assert_eq!(validate_string(string.as_ref()), Ok(()), "invalid string encountered: {:?}",string.as_ref());

		Self(string.into())
	}

	/// Gets a reference to the contained string.
	#[inline]
	pub fn as_str(&self) -> &str {
		self.0.as_ref()
	}
}

impl From<&str> for RcStr {
	/// Creates a new `RcStr` from the given `string`.
	///
	/// # Panics
	/// Panics if the given string is not valid.
	#[inline]
	fn from(string: &str) -> Self {
		Self::new(string).expect("invalid string given")
	}
}

impl From<String> for RcStr {
	/// Creates a new `RcStr` from the given `string`.
	///
	/// # Panics
	/// Panics if the given string is not valid.
	#[inline]
	fn from(string: String) -> Self {
		Self::new(string).expect("invalid string given")
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