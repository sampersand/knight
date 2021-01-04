use crate::{Value, Function};
use std::iter::Peekable;

#[derive(Debug, Clone, PartialEq, Eq)]
pub enum Ast {
	Value(Value),
	Identifier(String),
	Function(Function, Box<[Ast]>)
}

impl Ast {
	pub fn run(&self) -> Value {
		match self {
			Self::Value(value) => value.run(),
			Self::Identifier(identifier) =>
				if let Some(value) = crate::env::get(&identifier) {
					value.clone()
				} else {
					die!("unknown identifier '{}'", identifier);
				},
			Self::Function(func, args) => func.run(&args)
		}
	}

	fn parse_number(stream: &mut Peekable<impl Iterator<Item=char>>) -> Self {
		// note that negative integer literals aren't allowed in knight.
		let mut number: i128 = 0;

		while let Some(chr) = stream.peek() {
			if let Some(digit) = chr.to_digit(10) {
				let _ = stream.next();

				number *= 10;
				number += digit as i128;
			} else {
				break;
			}
		}

		Self::Value(Value::Number(number))
	}

	fn parse_identifier(stream: &mut Peekable<impl Iterator<Item=char>>) -> Self {
		let mut identifier = String::with_capacity(16); // most idents are more than 16 chars, so its good initial size.

		while let Some(chr) = stream.peek() {
			if matches!(chr, 'a'..='z' | '0'..='9' | '_') {
				identifier.push(*chr);
				let _ = stream.next();
			} else {
				break;
			}
		}

		debug_assert!(!identifier.is_empty());
		identifier.shrink_to_fit();

		Self::Identifier(identifier)
	}

	fn parse_string(stream: &mut Peekable<impl Iterator<Item=char>>) -> Self {
		let quote = stream.next().unwrap();
		let mut string = String::new();

		debug_assert!(quote == '\'' || quote == '\"');

		loop {
			match stream.next() {
				Some(chr) if chr == quote => break,
				Some(chr) => string.push(chr),
				None => die!("unterminated string encountered: {}{}", quote, string)
			}
		}

		string.shrink_to_fit();

		Self::Value(Value::String(string))
	}

	fn parse_function(stream: &mut Peekable<impl Iterator<Item=char>>) -> Self {
		let name = stream.next().unwrap();

		if name.is_uppercase() {
			while let Some(chr) = stream.peek() {
				if chr.is_uppercase() {
					let _ = stream.next();
				} else {
					break;
				}
			}
		}

		let function =
			if let Some(Some(function)) = name.is_ascii().then(|| Function::for_name(name as u8)) {
				function
			} else {
				die!("unknown function {:?}", name);
		};

		let mut args = Vec::with_capacity(function.arity());

		for _ in 0..function.arity() {
			args.push(Self::parse(stream));
		}

		Self::Function(function, args.into_boxed_slice())
	}

	pub fn parse(stream: &mut Peekable<impl Iterator<Item=char>>) -> Self {
		while let Some(chr) = stream.peek() {
			match chr {
				_ if chr.is_whitespace() => { stream.next(); },
				'(' | ')' | '[' | ']' | '{' | '}' | ':' => { stream.next(); },
				'#' =>
					while let Some(chr) = stream.next() {
						if chr == '\n' {
							break;
						}
					},
				'0'..='9' => return Self::parse_number(stream),
				'a'..='z' | '_' => return Self::parse_identifier(stream),
				'\"' | '\'' => return Self::parse_string(stream),
				_ => return Self::parse_function(stream)
			}
		}

		die!("expected an expression, hit EOF.");
	}
}

impl std::str::FromStr for Ast {
	type Err = std::convert::Infallible;

	fn from_str(string: &str) -> Result<Self, Self::Err> {
		Ok(Self::parse(&mut string.chars().peekable()))
	}
}
