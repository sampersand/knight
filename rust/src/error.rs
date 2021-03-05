use std::io;
use std::fmt::{self, Display, Formatter};

#[derive(Debug)]
pub enum ParseError {
	UnexpectedEOF,
	UnknownTokenStart { chr: char, lineno: usize },
	UnterminatedQuote { linestart: usize },
	MissingFunctionArgument { name: char, number: usize, lineno: usize }
}

#[derive(Debug)]
pub enum RuntimeError {
	DivisionByZero { modulo: bool },
	UnknownIdentifier(String),
	InvalidOperand { func: char, operand: &'static str },
	Parse(ParseError),
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
			Self::UnexpectedEOF => write!(f, "unexpected end of file encountered."),
			Self::UnknownTokenStart { chr, lineno } => write!(f, "line {}: unknown token start {:?}.", lineno, chr),
			Self::UnterminatedQuote { linestart } => write!(f, "line {}: unterminated quote encountered.", linestart),
			Self::MissingFunctionArgument { name, number, lineno }
				=> write!(f, "line {}: missing argument {} for function {:?}.", lineno, number, name)
		}
	}
}

impl std::error::Error for ParseError {}

impl Display for RuntimeError {
	fn fmt(&self, f: &mut Formatter) -> fmt::Result {
		match self {
			Self::DivisionByZero { modulo: false } => write!(f, "invalid divide by zero."),
			Self::DivisionByZero { modulo: true } => write!(f, "invalid modulo by zero."),
			Self::UnknownIdentifier(ident) => write!(f, "identifier {:?} is undefined.", ident),
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