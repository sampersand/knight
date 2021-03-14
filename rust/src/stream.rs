use crate::{Value, Number, Function, ParseError, error::ParseErrorKind};
use std::iter::Peekable;

#[derive(Debug, Clone)]
struct Stream<I: Iterator<Item=char>> {
	iter: Peekable<I>,
	cache: Option<char>,
	lineno: usize,
}


impl<I: Iterator<Item=char>> Iterator for Stream<I> {
	type Item = char;

	fn next(&mut self) -> Option<Self::Item> {
		let next = self.iter.next();

		if let Some('\n') = next {
			self.lineno += 1;
		}

		next
	}
}

fn whitespace(stream: &mut Stream<impl Iterator<Item=char>>) {
	while let Some(chr) = stream.iter.peek() {
		if chr.is_ascii_whitespace() {
			stream.next();
		} else {
			break;
		}
	}
}

fn comment(stream: &mut Stream<impl Iterator<Item=char>>) {
	while let Some(chr) = stream.next() {
		if chr == '\n' {
			break;
		}
	}
}

fn number(stream: &mut Stream<impl Iterator<Item=char>>) -> Value {
	let mut num: Number = 0;
	let mut is_negative = false;

	let chr = stream.next().unwrap();

	if let Some(digit) = chr.to_digit(10) {
		num = digit as _;
	} else {
		debug_assert_eq!(chr, '~');
		is_negative = true;
	}

	while let Some(chr) = stream.iter.peek() {
		if let Some(digit) = chr.to_digit(10) {
			num = num * 10 + (digit as Number);
			stream.next();
		} else {
			break;
		}
	}

	if is_negative {
		num = -num;
	}

	Value::Number(num)
}

fn identifier(stream: &mut Stream<impl Iterator<Item=char>>) -> Value {
	let mut ident = String::new();

	while let Some(&chr) = stream.iter.peek() {
		if chr.is_ascii_digit() || chr.is_ascii_lowercase() || chr == '_' {
			ident.push(chr);
			stream.next();
		} else {
			break;
		}
	}

	Value::Variable(ident)
}

fn string(stream: &mut Stream<impl Iterator<Item=char>>) -> Result<Value, ParseError> {
	let linestart = stream.lineno;

	let quote = stream.next().unwrap();
	let mut string = String::new();

	while let Some(chr) = stream.next() {
		if chr == quote {
			return Ok(Value::String(string.into()));
		} else {
			string.push(chr);
		}
	}

	parse_error!(UnterminatedQuote { linestart })
}

fn function(func: Function, stream: &mut Stream<impl Iterator<Item=char>>) -> Result<Value, ParseError> {
	let mut args = Vec::with_capacity(func.arity());
	let lineno = stream.lineno;

	if stream.next().unwrap().is_ascii_uppercase() {
		while stream.iter.peek().map_or(false, char::is_ascii_uppercase) {
			stream.next();
		}
	}

	for number in 0..func.arity() {
		match Value::parse_inner(stream).map_err(From::from) {
			Ok(value) => args.push(value),
			Err(ParseErrorKind::NothingToParse) =>
				return parse_error!(MissingFunctionArgument { func: func.name(), number, lineno }),
			Err(other) => return Err(other.into())
		}
	}

	Ok(Value::Function(func, args.into_boxed_slice().into()))
}

impl Value {
	pub fn parse_str<S: AsRef<str>>(input: S) -> Result<Self, ParseError> {
		Self::parse(input.as_ref().chars())
	}

	pub fn parse<S: Iterator<Item=char>>(input: S) -> Result<Self, ParseError> {
		Self::parse_inner(&mut Stream { iter: input.peekable(), lineno: 0, cache: None })
	}

	fn parse_inner(stream: &mut Stream<impl Iterator<Item=char>>) -> Result<Self, ParseError> {
		loop {
			let chr = *stream.iter.peek().ok_or_else(|| ParseError::from(ParseErrorKind::NothingToParse))?;

			match chr {
				// note that this is ascii whitespace, as non-ascii characters are invalid.
				_ if chr.is_ascii_whitespace() => whitespace(stream),

				// strip comments until eol.
				'#' => comment(stream),

				// ignore parens; consecutive ones aren't common, so dont deserve a function.
				'(' | ')' | '[' | ']' | '{' | '}' | ':' => { stream.iter.next(); },

				// only ascii digits and `~` may start number, where `~` is unary minus for literals
				'0'..='9' | '~' => return Ok(number(stream)),

				// identifiers start only with lower-case digits or `_`.
				'a'..='z' | '_' => return Ok(identifier(stream)),

				'T' | 'F' => {
					while let Some(c) = stream.iter.peek() {
						if c.is_ascii_uppercase() {
							stream.next();
						} else {
							break;
						}
					}

					return Ok(Value::Boolean(chr == 'T'))
				},

				'N' => {
					while let Some(chr) = stream.iter.peek() {
						if chr.is_ascii_uppercase() {
							stream.next();
						} else {
							break;
						}
					}

					return Ok(Value::Null)
				},
				
				// strings start with a single or double quote (and not `` ` ``).
				'\'' | '\"' => return string(stream),

				_ =>
					if let Some(func) = Function::fetch(chr) {
						return function(func, stream);
					} else {
						return parse_error!(UnknownTokenStart { chr, lineno: stream.lineno });
					}
			}
		}
	}
}