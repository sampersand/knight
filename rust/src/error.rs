use std::io;
use std::fmt::{self, Display, Formatter};
use crate::rcstr::InvalidChar;

/// The error type used to indicate an error whilst parsing Knight source code.
#[derive(Debug)]
pub enum ParseError {
	/// Indicates that the end of stream was reached before a value could be parsed.
	NothingToParse,

	/// Indicates that an invalid character was encountered.
	UnknownTokenStart {
		/// The invalid character that was encountered.
		chr: char,

		/// The line that the invalid character occurred on.
		line: usize
	},

	/// A starting quote was found without an associated ending quote.
	UnterminatedQuote {
		/// The line number the string started on.
		line: usize
	},

	/// A function was parsed, but one of its arguments was not able to be parsed.
	MissingFunctionArgument {
		/// The function whose argument is missing.
		func: char,

		/// The argument number.
		number: usize,

		/// The line number the function started on.
		line: usize
	},

	/// An invalid character was encountered in a [`RcStr`] literal.
	InvalidString {
		/// The line whence the string started.
		line: usize,

		/// The error itself.
		err: InvalidChar
	},
}

/// An error occurred whilst executing a knight program.
#[derive(Debug)]
pub enum RuntimeError {
	/// A division (or modulus) by zero was attempted.
	DivisionByZero {
		/// Whether or not its a modulus error.
		modulo: bool
	},

	/// An unknown identifier was attempted to be dereferenced.
	UnknownIdentifier {
		/// The identifier at fault.
		identifier: Box<str>
	},

	/// A function was executed with an invalid operand.
	InvalidOperand {
		/// The function that was attempted.
		func: char,

		/// The type of the operand.
		operand: &'static str
	},

	UndefinedConversion {
		into: &'static str,
		kind: &'static str
	},

	/// A checked operation failed.
	#[cfg(feature = "checked-overflow")]
	Overflow {
		/// Which function overflowed.
		func: char,

		/// The left-hand-side of the function.
		lhs: crate::Number,

		/// The right-hand-side of the function.
		rhs: crate::Number,
	},

	/// Exit with the given status code.
	Quit(i32),

	/// An error occurred whilst parsing (i.e. `EVAL` failed).
	Parse(ParseError),

	/// An invalid string was encountered.
	InvalidString(InvalidChar),

	/// An i/o error occurred (i.e. `` ` `` or `PROMPT` failed).
	Io(io::Error),
}

impl From<ParseError> for RuntimeError {
	#[inline]
	fn from(err: ParseError) -> Self {
		Self::Parse(err)
	}
}

impl From<io::Error> for RuntimeError {
	#[inline]
	fn from(err: io::Error) -> Self {
		Self::Io(err)
	}
}

impl From<InvalidChar> for RuntimeError {
	#[inline]
	fn from(err: InvalidChar) -> Self {
		Self::InvalidString(err)
	}
}

impl Display for ParseError {
	fn fmt(&self, f: &mut Formatter) -> fmt::Result {
		match self {
			Self::NothingToParse => write!(f, "a token was expected."),
			Self::UnknownTokenStart { chr, line } => write!(f, "line {}: unknown token start {:?}.", line, chr),
			Self::UnterminatedQuote { line } => write!(f, "line {}: unterminated quote encountered.", line),
			Self::MissingFunctionArgument { func, number, line }
				=> write!(f, "line {}: missing argument {} for function {:?}.", line, number, func),
			Self::InvalidString { line, err } => write!(f, "line {}: {}", line, err)
		}
	}
}

impl std::error::Error for ParseError {
	fn source(&self) -> Option<&(dyn std::error::Error + 'static)> {
		match self {
			Self::InvalidString { err, .. } => Some(err),
			_ => None
		}
	}
}

impl Display for RuntimeError {
	fn fmt(&self, f: &mut Formatter) -> fmt::Result {
		match self {
			Self::DivisionByZero { modulo: false } => write!(f, "invalid divide by zero."),
			Self::DivisionByZero { modulo: true } => write!(f, "invalid modulo by zero."),
			Self::UnknownIdentifier { identifier } => write!(f, "identifier {:?} is undefined.", identifier),
			Self::InvalidOperand { func, operand } => write!(f, "invalid operand kind {:?} for function {:?}.", operand, func),
			Self::UndefinedConversion { kind, into } => write!(f, "invalid conversion into {:?} for kind {:?}.", kind, into),
			#[cfg(feature = "checked-overflow")]
			Self::Overflow { func, lhs, rhs } => write!(f, "Expression '{} {} {}' overflowed", lhs, func, rhs),
			Self::Quit(code) => write!(f, "exit with status {}", code),
			Self::Parse(err) => Display::fmt(err, f),
			Self::InvalidString(err) => Display::fmt(err, f),
			Self::Io(err) => write!(f, "i/o error: {}", err)
		}
	}
}

impl std::error::Error for RuntimeError {
	fn source(&self) -> Option<&(dyn std::error::Error + 'static)> {
		match self {
			Self::Parse(err) => Some(err),
			Self::Io(err) => Some(err),
			Self::InvalidString(err) => Some(err),
			_ => None
		}
	}
}