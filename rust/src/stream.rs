use crate::{Value, Number, Function, ParseError};
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

	Value::Variable(crate::Variable::from(ident))
}

fn string(stream: &mut Stream<impl Iterator<Item=char>>) -> Result<Value, ParseError> {
	let linestart = stream.lineno;

	let quote = stream.next().unwrap();
	let mut string = String::new();
	debug_assert!(quote == '\'' || quote == '\"');

	while let Some(chr) = stream.next() {
		if chr == quote {
			return Ok(Value::String(string.into()));
		} else {
			string.push(chr);
		}
	}

	Err(ParseError::UnterminatedQuote { linestart })
}

fn function(func: Function, stream: &mut Stream<impl Iterator<Item=char>>) -> Result<Value, ParseError> {
	let mut args = Vec::with_capacity(func.arity());
	let lineno = stream.lineno;

	if stream.next().unwrap().is_ascii_uppercase() {
		strip_word(stream);
	}

	for number in 0..func.arity() {
		match Value::parse_inner(stream) {
			Ok(value) => args.push(value),
			Err(ParseError::NothingToParse) =>
				return Err(ParseError::MissingFunctionArgument { func: func.name(), number, lineno }),
			Err(other) => return Err(other)
		}
	}

	Ok(Value::Function(func, args.into_boxed_slice().into()))
}

fn strip_word(stream: &mut Stream<impl Iterator<Item=char>>) {
	while stream.iter.peek().map_or(false, |c| c.is_ascii_uppercase() || *c == '_') {
		stream.next();
	}
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
			match *stream.iter.peek().ok_or(ParseError::NothingToParse)? {
				// note that this is ascii whitespace, as non-ascii characters are invalid.
				chr if chr.is_ascii_whitespace() => whitespace(stream),

				// strip comments until eol.
				'#' => comment(stream),

				// ignore parens; consecutive ones aren't common, so dont deserve a function.
				'(' | ')' | '[' | ']' | '{' | '}' | ':' => { stream.iter.next(); },

				// only ascii digits and `~` may start number, where `~` is unary minus for literals
				'0'..='9' | '~' => return Ok(number(stream)),

				// identifiers start only with lower-case digits or `_`.
				'a'..='z' | '_' => return Ok(identifier(stream)),

				chr @ 'T' | chr @ 'F' => { strip_word(stream); return Ok(Value::Boolean(chr == 'T')); },

				'N' => { strip_word(stream); return Ok(Value::Null); },
				
				// strings start with a single or double quote (and not `` ` ``).
				'\'' | '\"' => return string(stream),

				chr =>
					if let Some(func) = Function::fetch(chr) {
						return function(func, stream);
					} else {
						return Err(ParseError::UnknownTokenStart { chr, lineno: stream.lineno });
					}
			}
		}
	}
}