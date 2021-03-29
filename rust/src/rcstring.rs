//! Types relating to the [`RcString`].

use std::sync::Arc;
use std::fmt::{self, Debug, Display, Formatter};
use std::hash::{Hash, Hasher};
use std::convert::TryFrom;

/// The string type within Knight.
#[derive(Clone)]
pub struct RcString(Arc<str>);

impl Default for RcString {
	fn default() -> Self {
		use once_cell::sync::OnceCell;

		static EMPTY: OnceCell<RcString> = OnceCell::new();

		EMPTY.get_or_init(|| unsafe { Self::new_unchecked("") }).clone()
	}
}

impl Debug for RcString {
	fn fmt(&self, f: &mut Formatter) -> fmt::Result {
		Debug::fmt(self.as_str(), f)
	}
}

impl Display for RcString {
	fn fmt(&self, f: &mut Formatter) -> fmt::Result {
		Display::fmt(self.as_str(), f)
	}
}

impl Hash for RcString {
	fn hash<H: Hasher>(&self, h: &mut H) {
		self.as_str().hash(h)
	}
}

impl Eq for RcString {}
impl PartialEq for RcString {
	fn eq(&self, rhs: &Self) -> bool {
		self.as_str() == rhs.as_str()
	}
}

impl PartialOrd for RcString {
	fn partial_cmp(&self, rhs: &Self) -> Option<std::cmp::Ordering> {
		self.as_str().partial_cmp(rhs.as_str())
	}
}

impl Ord for RcString {
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
		write!(f, "invalid byte {:?} found at position {}", self.chr, self.idx)
	}
}

impl std::error::Error for InvalidChar {}

/// Checks to see if `chr` is a valid knight character.
pub fn is_valid_char(chr: char) -> bool {
	return matches!(chr, '\r' | '\n' | '\t' | ' '..='~');
}

fn validate_string(data: &str) -> Result<(), InvalidChar> {
	for (idx, chr) in data.chars().enumerate() {
		if !is_valid_char(chr) {
			return Err(InvalidChar { chr, idx });
		}
	}

	Ok(())
}

impl RcString {
	/// Creates a new `RcString` with the given input string.
	///
	/// # Errors
	/// If `string` contains any characters which aren't valid in Knight source code, an `InvalidChar` is returned.
	///
	/// # See Also
	/// - [`RcString::new_unchecked`] For a version which doesn't verify `string`.
	pub fn new<T: ToString>(string: T) -> Result<Self, InvalidChar> {
		let string = string.to_string();
		validate_string(&string)?;

		// SAFETY: we just validated the string.
		unsafe {
			Ok(Self::new_unchecked(string))
		}
	}

	/// Creates a new `RcString`, without verifying that the string is valid.
	///
	/// # Safety
	/// All characters within the string must be valid for Knight strings. See the specs for what exactly this entails.
	pub unsafe fn new_unchecked<T: ToString>(string: T) -> Self {
		let string = string.to_string();

		debug_assert_eq!(validate_string(&string), Ok(()), "invalid string encountered: {:?}", &string);

		Self(string.into())
	}

	/// Gets a reference to the contained string.
	#[inline]
	pub fn as_str(&self) -> &str {
		self.0.as_ref()
	}
}

impl TryFrom<&str> for RcString {
	type Error = InvalidChar;

	#[inline]
	fn try_from(string: &str) -> Result<Self, Self::Error> {
		Self::new(string)
	}
}

impl TryFrom<String> for RcString {
	type Error = InvalidChar;

	#[inline]
	fn try_from(string: String) -> Result<Self, Self::Error> {
		Self::new(string)
	}
}

impl AsRef<str> for RcString {
	#[inline]
	fn as_ref(&self) -> &str {
		self.as_str()
	}
}

impl std::ops::Deref for RcString {
	type Target = str;

	#[inline]
	fn deref(&self) -> &Self::Target {
		self.as_str()
	}
}