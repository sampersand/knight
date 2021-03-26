use std::io;
use std::fmt::{self, Display, Formatter};

/// The error type used to indicate an error whilst parsing Knight source code.
#[derive(Debug)]
pub enum ParseError {
	/// Indicates that the end of stream was reached before a value could be parsed.
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
	}
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
		identifier: &'static str
	},

	/// A function was executed with an invalid operand.
	InvalidOperand {
		/// The function that was attempted.
		func: char,

		/// The type of the operand.
		operand: &'static str
	},

	/// An error occurred whilst parsing (i.e. `EVAL` failed).
	Parse(ParseError),

	/// An i/o error occurred (i.e. `` ` `` or `PROMPT` failed).
	Io(io::Error)
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

impl Display for ParseError {
	fn fmt(&self, f: &mut Formatter) -> fmt::Result {
		match self {
			Self::NothingToParse => write!(f, "a token was expected."),
			Self::UnknownTokenStart { chr, lineno } => write!(f, "line {}: unknown token start {:?}.", lineno, chr),
			Self::UnterminatedQuote { linestart } => write!(f, "line {}: unterminated quote encountered.", linestart),
			Self::MissingFunctionArgument { func, number, lineno }
				=> write!(f, "line {}: missing argument {} for function {:?}.", lineno, number, func)
		}
	}
}

impl std::error::Error for ParseError {}

impl Display for RuntimeError {
	fn fmt(&self, f: &mut Formatter) -> fmt::Result {
		match self {
			Self::DivisionByZero { modulo: false } => write!(f, "invalid divide by zero."),
			Self::DivisionByZero { modulo: true } => write!(f, "invalid modulo by zero."),
			Self::UnknownIdentifier { identifier } => write!(f, "identifier {:?} is undefined.", identifier),
			Self::InvalidOperand { func, operand } => write!(f, "invalid operand kind {:?} for function {:?}.", operand, func),
			Self::Parse(err) => Display::fmt(err, f),
			Self::Io(err) => write!(f, "i/o error: {}", err)
		}
	}
}

impl std::error::Error for RuntimeError {
	fn source(&self) -> Option<&(dyn std::error::Error + 'static)> {
		match self {
			Self::Parse(err) => Some(err),
			Self::Io(err) => Some(err),
			_ => None
		}
	}
}