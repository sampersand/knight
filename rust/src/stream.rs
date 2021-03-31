use crate::{Value, Number, Function, ParseError, Environment, RcString, Variable};
use std::convert::TryFrom;

#[derive(Debug, Clone)]
pub struct Stream<I: Iterator<Item=char>> {
	iter: I,
	prev: Option<char>,
	rewound: bool,
	line: usize,
}

impl<I: Iterator<Item=char>> Stream<I> {
	pub fn new(iter: I) -> Self {
		Self {
			iter,
			prev: None,
			rewound: false,
			line: 1 // start on line 1
		}
	}

	pub fn rewind(&mut self) {
		assert!(!self.rewound);
		assert!(self.prev.is_some());

		self.rewound = true;

		if self.prev == Some('\n') {
			self.line -= 1;
		}
	}

	pub fn prev(&self) -> Option<char> {
		self.prev
	}

	pub fn peek(&mut self) -> Option<char> {
		let next = self.next();
		self.rewind();
		next
	}
}

impl<I: Iterator<Item=char>> Iterator for Stream<I> {
	type Item = char;

	fn next(&mut self) -> Option<Self::Item> {
		if self.rewound {
			self.rewound = false;
		} else {
			self.prev = self.iter.next();
		}

		if self.prev == Some('\n') {
			self.line += 1;
		}

		self.prev
	}
}

fn is_whitespace(chr: char) -> bool {
	matches!(chr, ' ' | '\n' | '\r' | '\t' | '(' | ')' | '[' | ']' | '{' | '}' | ':' )
}

impl<I: Iterator<Item=char>> Stream<I> {
	pub fn try_whitespace(&mut self) -> bool {
		if self.peek().map_or(false, is_whitespace) {
			unsafe { self.whitespace_unchecked() };
			true
		} else {
			false
		}
	}

	pub unsafe fn whitespace_unchecked(&mut self) {
		if cfg!(debug_assertions) {
			match self.peek() {
				Some(chr) if is_whitespace(chr) => {},
				Some(other) => panic!("start character '{:?}' is not whitespace", other),
				None => panic!("encountered end of stream")
			}
		}


		while let Some(chr) = self.next() {
			if !is_whitespace(chr) {
				self.rewind();
				break;
			}
		}
	}

	pub fn try_comment(&mut self) -> bool {
		if self.peek() == Some('#') {
			unsafe { self.comment_unchecked() };
			true
		} else {
			false
		}
	}

	pub unsafe fn comment_unchecked(&mut self) {
		debug_assert_eq!(self.peek(), Some('#'), "stream doesn't start with a '#'");

		for chr in self {
			if chr == '\n' {
				break;
			}
		}
	}

	pub fn try_number(&mut self) -> Option<Number> {
		if matches!(self.peek(), Some('0'..='9')) {
			Some(unsafe { self.number_unchecked() })
		} else {
			None
		}
	}

	pub unsafe fn number_unchecked(&mut self) -> Number {
		if cfg!(debug_assertions) {
			match self.peek() {
				Some('0'..='9') => {},
				Some(other) => panic!("start character '{:?}' is not a digit", other),
				None => panic!("encountered end of stream")
			}
		}

		let mut number: Number = 0;

		while let Some(digit) = self.next() {
			if !digit.is_ascii_digit() {
				self.rewind();
				break;
			}

			number *= 10;
			number += (digit as u8 - b'0') as Number;
		}

		number
	}

	pub fn try_variable(&mut self, env: &mut Environment<'_, '_, '_>) -> Option<Variable> {
		if matches!(self.peek(), Some('a'..='z') | Some('_')) {
			Some(unsafe { self.variable_unchecked(env) })
		} else {
			None
		}
	}

	pub unsafe fn variable_unchecked(&mut self, env: &mut Environment<'_, '_, '_>) -> Variable {
		if cfg!(debug_assertions) {
			match self.peek() {
				Some('a'..='z') | Some('_') => {},
				Some(other) => panic!("start character '{:?}' is not a valid identifier start", other),
				None => panic!("encountered end of stream")
			}
		}

		let mut ident = String::new();

		while let Some(chr) = self.next() {
			if chr.is_ascii_digit() || chr.is_ascii_lowercase() || chr == '_' {
				ident.push(chr);
			} else {
				self.rewind();
				break;
			}
		}

		env.get(&ident)
	}

	pub fn try_string(&mut self) -> Option<Result<RcString, ParseError>> {
		if matches!(self.peek(), Some('\'') | Some('\"')) {
			Some(unsafe { self.string_unchecked() })
		} else {
			None
		}
	}

	pub unsafe fn string_unchecked(&mut self) -> Result<RcString, ParseError> {
		let line = self.line;
		let quote = 
			match self.next() {
				Some(quote @ '\'') | Some(quote @ '\"') => quote,
				Some(other) if cfg!(debug_assertions) => panic!("character {:?} is not '\\'' or '\\\"'", other),
				None if cfg!(debug_assertions) => panic!("encountered end of stream"),
				_ => unsafe { std::hint::unreachable_unchecked() }
			};

		let mut string = String::new();

		for chr in self {
			if chr == quote {
				return RcString::try_from(string).map_err(|err| ParseError::InvalidString { line, err });
			}

			string.push(chr);
		}

		Err(ParseError::UnterminatedQuote { line })
	}

	pub fn try_boolean(&mut self) -> Option<bool> {
		if matches!(self.peek(), Some('T') | Some('F')) {
			Some(unsafe { self.boolean_unchecked() })
		} else {
			None
		}
	}

	pub unsafe fn boolean_unchecked(&mut self) -> bool {
		let is_true =
			match self.next() {
				Some('T') => true,
				Some('F') => false,
				Some(other) if cfg!(debug_assertions) => panic!("character {:?} is not 'T' or 'F'", other),
				None if cfg!(debug_assertions) => panic!("encountered end of stream"),
				_ => unsafe { std::hint::unreachable_unchecked() }
			};

		self.strip_word();

		is_true
	}

	pub fn try_null(&mut self) -> bool {
		if self.peek() == Some('N') {
			unsafe { self.null_unchecked(); }
			true
		} else {
			false
		}
	}

	pub unsafe fn null_unchecked(&mut self) {
		match self.next() {
			Some('N') => self.strip_word(),
			Some(other) if cfg!(debug_assertions) => panic!("character {:?} is not 'N'", other),
			None if cfg!(debug_assertions) => panic!("encountered end of stream"),
			_ => unsafe { std::hint::unreachable_unchecked() }
		}
	}

	pub fn function(&mut self, func: Function, env: &mut Environment<'_, '_, '_>) -> Result<Value, ParseError> {
		let mut args = Vec::with_capacity(func.arity());
		let line = self.line;

		if func.name().is_ascii_uppercase() {
			self.strip_word();
		}

		for number in 0..func.arity() {
			match self.parse(env) {
				Ok(value) => args.push(value),
				Err(ParseError::NothingToParse) =>
					return Err(ParseError::MissingFunctionArgument { func: func.name(), number, line }),
				Err(other) => return Err(other)
			}
		}

		Ok(Value::Function(func, args.into_boxed_slice().into()))
	}

	fn strip_word(&mut self) {
		while let Some(chr) = self.next() {
			if !matches!(chr, 'A'..='Z' | '_') {
				self.rewind();
				return;
			}
		}
	}

	pub fn parse(&mut self, env: &mut Environment<'_, '_, '_>) -> Result<Value, ParseError> {
		match self.peek().ok_or(ParseError::NothingToParse)? {
			// note that this is ascii whitespace, as non-ascii characters are invalid.
			' ' | '\n' | '\r' | '\t' | '(' | ')' | '[' | ']' | '{' | '}' | ':' => {
				unsafe { self.whitespace_unchecked(); }
				self.parse(env)
			},

			// strip comments until eol.
			'#' => {
				unsafe { self.comment_unchecked(); }
				self.parse(env)
			},

			// only ascii digits may start a number.
			'0'..='9' => Ok(Value::Number(unsafe { self.number_unchecked() })),

			// identifiers start only with lower-case digits or `_`.
			'a'..='z' | '_' => Ok(Value::Variable(unsafe { self.variable_unchecked(env) })),

			'T' | 'F' => Ok(Value::Boolean(unsafe { self.boolean_unchecked() })),

			'N' => { unsafe { self.null_unchecked(); }; Ok(Value::Null) },
			
			// strings start with a single or double quote (and not `` ` ``).
			'\'' | '\"' => Ok(Value::String(unsafe { self.string_unchecked()? })),

			chr => 
				if let Some(func) = Function::fetch(chr) {
					self.next();

					self.function(func, env)
				} else {
					Err(ParseError::UnknownTokenStart { chr, line: self.line })
				}
		}
	}
}

impl Value {
	/// Parses out a stream from the given `input` within the context of `env`.
	///
	/// This function simply calls [`parse`] with a char iterator over `input`; see it for more details.
	///
	/// # Errors
	/// This function returns any errors that [`parse`] returns.
	pub fn parse_str<S: AsRef<str>>(input: S, env: &mut Environment<'_, '_, '_>) -> Result<Self, ParseError> {
		Self::parse(input.as_ref().chars(), env)
	}

	/// Parses out a stream from the given `input` within the context of `env`.
	///
	/// Note: Yes, technically this could be an iterator over `u8`, as the Knight specs clearly state that all source
	/// bytes are a subset of ASCII. However, we may want to support fun stuff like non-ASCII variables as an optional
	/// extension in the future. As such, `char` is required.
	///
	/// # Errors
	/// This function returns any errors that occur whilst parsing; See [`ParseError`]'s variants for what conditions can
	/// cause errors.
	///
	/// # See Also
	/// Section 1. within the Knight specs for parsing.
	pub fn parse<S: IntoIterator<Item=char>>(input: S, env: &mut Environment<'_, '_, '_>) -> Result<Self, ParseError> {
		Stream::new(input.into_iter()).parse(env)
	}
}
