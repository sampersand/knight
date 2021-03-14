use std::io;
use crate::rcstr::InvalidString;
use std::fmt::{self, Display, Formatter};

#[derive(Debug)]
#[cfg(not(feature = "fatal-errors"))]
pub struct ParseError(ParseErrorKind);

#[derive(Debug)]
#[cfg(feature = "fatal-errors")]
pub struct ParseError(std::convert::Infallible);

/// The error type used to indicate an error whilst parsing Knight source code.
#[derive(Debug)]
pub enum ParseErrorKind {
	/// Indicates the entire stream was simply whitespace and comments
	NothingToParse,
	/// Indicates that an invalid character was encountered.
	UnknownTokenStart {
		/// The invalid character that.
		chr: char,
		/// The line that the invalid character occurred on.
		lineno: usize
	},
	/// A starting quote was found without an associated ending quote.
	UnterminatedQuote {
		/// The line number the string started on.
		linestart: usize
	},
	/// A function was parsed, but one of its arguments was not able to be parsed.
	MissingFunctionArgument {
		/// The function whose argument is missing.
		func: char,
		/// The argument number.
		number: usize,
		/// The line number the function started on.
		lineno: usize
	},

	BadSourceyte { err: InvalidString, lineno: usize },
}

impl From<ParseErrorKind> for ParseError {
	#[cfg_attr(feature = "fatal-errors", inline)]
	fn from(kind: ParseErrorKind) -> Self {
		cfg_if! {
			if #[cfg(feature = "fatal-errors")] {
				let _ = kind;
				unsafe { unreachable_unchecked!(); }
			} else {
				Self(kind)
			}
		}
	}
}

impl From<ParseError> for ParseErrorKind {
	#[cfg_attr(feature = "fatal-errors", inline)]
	fn from(err: ParseError) -> Self {
		cfg_if! {
			if #[cfg(feature = "fatal-errors")] {
				let _ = err;
				unsafe { unreachable_unchecked!(); }
			} else {
				err.0
			}
		}
	}
}

#[derive(Debug)]
#[cfg(not(feature = "fatal-errors"))]
pub struct RuntimeError(RuntimeErrorKind);

#[derive(Debug)]
#[cfg(feature = "fatal-errors")]
pub struct RuntimeError(std::convert::Infallible);

/// An error occurred whilst executing a knight program.
#[derive(Debug)]
pub enum RuntimeErrorKind {
	/// A division (or modulus) by zero was attempted.
	DivisionByZero {
		/// Whether or not its a modulus error.
		modulo: bool
	},
	/// An unknown identifier was attempted to be dereferenced.
	UnknownIdentifier {
		/// The identifier at fault.
		identifier: String
	},
	/// A function was executed with an invalid operand.
	InvalidOperand {
		/// The function that was attempted.
		func: char,
		/// The type of the operand.
		operand: &'static str
	},

	InvalidString(InvalidString),

	/// An error occurred whilst parsing (i.e. `EVAL` failed.)
	Parse(ParseError),

	/// An i/o error occurred (i.e. `` ` `` or `PROMPT` failed).
	Io(io::Error)
}

impl From<RuntimeErrorKind> for RuntimeError {
	#[cfg_attr(feature = "fatal-errors", inline)]
	fn from(kind: RuntimeErrorKind) -> RuntimeError {
		cfg_if! {
			if #[cfg(feature = "fatal-errors")] {
				let _ = kind;
				unsafe { unreachable_unchecked!(); }
			} else {
				Self(kind)
			}
		}
	}
}

impl From<RuntimeError> for RuntimeErrorKind {
	#[cfg_attr(feature = "fatal-errors", inline)]
	fn from(err: RuntimeError) -> RuntimeErrorKind {
		cfg_if! {
			if #[cfg(feature = "fatal-errors")] {
				let _ = err;
				unsafe { unreachable_unchecked!(); }
			} else {
				err.0
			}
		}
	}
}

impl From<InvalidString> for RuntimeError {
	#[cfg_attr(not(feature = "fatal-errors"), inline)]
	fn from(err: InvalidString) -> Self {
		cfg_if! {
			if #[cfg(feature = "reckless")] {
				let _ = err;
				unsafe { unreachable_unchecked!(); }
			} else if #[cfg(feature = "fatal-errors")] {
				panic!("invalid string: {}", err);
			} else {
				Self(RuntimeErrorKind::InvalidString(err))
			}
		}
	}
}

impl From<ParseError> for RuntimeError {
	#[cfg_attr(not(feature = "fatal-errors"), inline)]
	fn from(err: ParseError) -> Self {
		cfg_if! {
			if #[cfg(feature = "reckless")] {
				let _ = err;
				unsafe { unreachable_unchecked!(); }
			} else if #[cfg(feature = "fatal-errors")] {
				panic!("parse error encountered: {}", err);
			} else {
				Self(RuntimeErrorKind::Parse(err))
			}
		}
	}
}

impl From<io::Error> for RuntimeError {
	#[cfg_attr(not(feature = "fatal-errors"), inline)]
	fn from(err: io::Error) -> Self {
		cfg_if! {
			if #[cfg(feature = "reckless")] {
				let _ = err;
				unsafe { unreachable_unchecked!(); }
			} else if #[cfg(feature = "fatal-errors")] {
				panic!("io error encountered: {}", err);
			} else {
				Self(RuntimeErrorKind::Io(err))
			}
		}
	}
}

impl Display for ParseError {
	fn fmt(&self, f: &mut Formatter) -> fmt::Result {
		cfg_if! {
			if #[cfg(feature = "fatal-errors")] {
				let _ = f;
				unsafe { unreachable_unchecked!(); }
			} else {
				match self.0 {
					ParseErrorKind::NothingToParse => write!(f, "a token was expected."),
					ParseErrorKind::UnknownTokenStart { chr, lineno } =>
						write!(f, "line {}: unknown token start {:?}.", lineno, chr),
					ParseErrorKind::UnterminatedQuote { linestart } =>
						write!(f, "line {}: unterminated quote encountered.", linestart),
					ParseErrorKind::MissingFunctionArgument { func, number, lineno } =>
						write!(f, "line {}: missing argument {} for function {:?}.", lineno, number, func),
					ParseErrorKind::BadSourceyte { ref err, lineno } => write!(f, "line {}: {}", lineno, err)
				}
			}
		}
	}
}


impl std::error::Error for ParseError {
	fn source(&self) -> Option<&(dyn std::error::Error + 'static)> {
		cfg_if! {
			if #[cfg(feature = "fatal-errors")] {
				unsafe { unreachable_unchecked!(); }
			} else {
				if let ParseErrorKind::BadSourceyte { ref err, .. } = self {
					Some(err)
				} else {
					None
				}
			}
		}
	}
}
impl Display for RuntimeError {
	fn fmt(&self, f: &mut Formatter) -> fmt::Result {
		#[cfg(feature = "fatal-errors")]
		{
			let _ = f;
			unsafe { unreachable_unchecked!(); }
		}

		#[cfg(not(feature = "fatal-errors"))]
		{
			match &self.0 {
				RuntimeErrorKind::DivisionByZero { modulo: false } => write!(f, "invalid divide by zero."),
				RuntimeErrorKind::DivisionByZero { modulo: true } => write!(f, "invalid modulo by zero."),
				RuntimeErrorKind::UnknownIdentifier { identifier } => write!(f, "identifier {:?} is undefined.", identifier),
				RuntimeErrorKind::InvalidOperand { func, operand } => write!(f, "invalid operand kind {:?} for function {:?}.", operand, func),
				RuntimeErrorKind::Parse(err) => Display::fmt(&err, f),
				RuntimeErrorKind::Io(err) => write!(f, "i/o error: {}", err),
				RuntimeErrorKind::InvalidString(err) => write!(f, "invalid string: {}", err)
			}
		}
	}
}

impl std::error::Error for RuntimeError {
	fn source(&self) -> Option<&(dyn std::error::Error + 'static)> {
		#[cfg(feature = "fatal-errors")]
		{
			unsafe { unreachable_unchecked!(); }
		}

		#[cfg(not(feature = "fatal-errors"))]
		{
			match &self.0 {
				RuntimeErrorKind::Parse(err) => Some(err),
				RuntimeErrorKind::Io(err) => Some(err),
				RuntimeErrorKind::InvalidString(err) => Some(err),
				_ => None
			}
		}
	}
}