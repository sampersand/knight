use crate::{Value, Number, Function, ParseError, Environment, RcString};
use std::iter::Peekable;
use std::convert::TryFrom;

#[derive(Debug, Clone)]
struct Stream<I: Iterator<Item=char>> {
	iter: Peekable<I>,
	cache: Option<char>,
	line: usize,
}

impl<I: Iterator<Item=char>> Iterator for Stream<I> {
	type Item = char;

	fn next(&mut self) -> Option<Self::Item> {
		let next = self.iter.next();

		if let Some('\n') = next {
			self.line += 1;
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
	for chr in stream {
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

fn identifier(stream: &mut Stream<impl Iterator<Item=char>>, env: &mut Environment<'_, '_, '_>) -> Value {
	let mut ident = String::new();

	while let Some(&chr) = stream.iter.peek() {
		if chr.is_ascii_digit() || chr.is_ascii_lowercase() || chr == '_' {
			ident.push(chr);
			stream.next();
		} else {
			break;
		}
	}

	Value::Variable(env.get(&ident))
}

fn string(stream: &mut Stream<impl Iterator<Item=char>>) -> Result<Value, ParseError> {
	let line = stream.line;

	let quote = stream.next().unwrap();
	let mut string = String::new();
	debug_assert!(quote == '\'' || quote == '\"');

	for chr in stream {
		if chr == quote {
			return RcString::try_from(string).map(Value::String).map_err(|err| ParseError::InvalidString { line, err });
		}

		string.push(chr);
	}

	Err(ParseError::UnterminatedQuote { line })
}

fn function(func: Function, stream: &mut Stream<impl Iterator<Item=char>>, env: &mut Environment<'_, '_, '_>)
	-> Result<Value, ParseError>
{
	let mut args = Vec::with_capacity(func.arity());
	let line = stream.line;

	if stream.next().unwrap().is_ascii_uppercase() {
		strip_word(stream);
	}

	for number in 0..func.arity() {
		match Value::parse_inner(stream, env) {
			Ok(value) => args.push(value),
			Err(ParseError::NothingToParse) =>
				return Err(ParseError::MissingFunctionArgument { func: func.name(), number, line }),
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
	/// bytes are a subset of ASCII. However, we may want to support fun stuff like non-ASCII identifiers as an optional
	/// extension in the future. As such, `char` is required.
	///
	/// # Errors
	/// This function returns any errors that occur whilst parsing; See [`ParseError`]'s variants for what conditions can
	/// cause errors.
	///
	/// # See Also
	/// Section 1. within the Knight specs for parsing.
	pub fn parse<S: IntoIterator<Item=char>>(input: S, env: &mut Environment<'_, '_, '_>) -> Result<Self, ParseError> {
		let mut stream =
			Stream {
				iter: input.into_iter().peekable(),
				line: 1,
				cache: None
			};

		Self::parse_inner(&mut stream, env)
	}

	fn parse_inner(
		stream: &mut Stream<impl Iterator<Item=char>>,
		env: &mut Environment<'_, '_, '_>
	) -> Result<Self, ParseError> {
		match *stream.iter.peek().ok_or(ParseError::NothingToParse)? {
			// note that this is ascii whitespace, as non-ascii characters are invalid.
			' ' | '\n' | '\r' | '\t' => { whitespace(stream); Self::parse_inner(stream, env) },

			// strip comments until eol.
			'#' => { comment(stream); Self::parse_inner(stream, env) },

			// ignore parens; consecutive ones aren't common, so dont deserve a function.
			'(' | ')' | '[' | ']' | '{' | '}' | ':' => { stream.iter.next(); Self::parse_inner(stream, env) },

			// only ascii digits and `~` may start number, where `~` is unary minus for literals
			'0'..='9' | '~' => Ok(number(stream)),

			// identifiers start only with lower-case digits or `_`.
			'a'..='z' | '_' => Ok(identifier(stream, env)),

			chr @ 'T' | chr @ 'F' => { strip_word(stream); Ok(Self::Boolean(chr == 'T')) },

			'N' => { strip_word(stream); Ok(Self::Null) },
			
			// strings start with a single or double quote (and not `` ` ``).
			'\'' | '\"' => string(stream),

			chr => 
				if let Some(func) = Function::fetch(chr) {
					function(func, stream, env)
				} else {
					Err(ParseError::UnknownTokenStart { chr, line: stream.line })
				}
		}
	}
}