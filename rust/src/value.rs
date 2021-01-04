use std::borrow::Cow;
use std::ops;
use crate::Ast;

pub type Integer = i128;

#[derive(Debug, Clone, PartialEq, Eq)]
pub enum Value {
	Null,
	Boolean(bool),
	Number(Integer),
	String(String),
	Ast(Box<Ast>)
}

impl Value {
	pub fn as_boolean(&self) -> bool {
		From::from(self)
	}

	pub fn as_number(&self) -> Integer {
		From::from(self)
	}

	pub fn as_string(&self) -> Cow<str> {
		From::from(self)
	}
}

impl From<&Value> for bool {
	fn from(value: &Value) -> Self {
		match value {
			Value::Null => false,
			Value::Boolean(boolean) => *boolean,
			Value::Number(number) => *number != 0,
			Value::String(string) => !string.is_empty(),
			Value::Ast(ast) => From::from(&ast.run())
		}
	}
}

impl From<&Value> for Integer {
	fn from(value: &Value) -> Self {
		match value {
			Value::Null => 0,
			Value::Boolean(false) => 0,
			Value::Boolean(true) => 1,
			Value::Number(number) => *number,
			Value::String(string) => {
				let mut string = string.trim();
				let is_negative = string.chars().nth(0) == Some('-');

				if is_negative {
					string = string.get(1..).unwrap();
				}

				(if is_negative { -1 } else { 1 }) * match string.find(|c: char| !c.is_ascii_digit()) {
					Some(0) => 0,
					Some(idx) => string.get(..idx).unwrap().parse().unwrap(),
					None if string.is_empty() => 0,
					None => string.parse().unwrap()
				}
			},
			Value::Ast(ast) => From::from(&ast.run())
		}
	}
}

impl<'a> From<&'a Value> for Cow<'a, str>  {
	fn from(value: &'a Value) -> Self {
		match value {
			Value::Null => Cow::Borrowed("null"),
			Value::Boolean(true) => Cow::Borrowed("true"),
			Value::Boolean(false) => Cow::Borrowed("false"),
			Value::Number(number) => Cow::Owned(number.to_string()),
			Value::String(string) => Cow::Borrowed(string.as_ref()),
			Value::Ast(ast) => Cow::from(&ast.run()).into_owned().into()
		}
	}
}

impl From<()> for Value {
	#[inline]
	fn from(_: ()) -> Self {
		Self::Null
	}
}

impl From<bool> for Value {
	#[inline]
	fn from(value: bool) -> Self {
		Self::Boolean(value)
	}
}

impl From<Integer> for Value {
	#[inline]
	fn from(integer: Integer) -> Self {
		Self::Number(integer)
	}
}

impl From<String> for Value {
	#[inline]
	fn from(string: String) -> Self {
		Self::String(string)
	}
}

impl From<Ast> for Value {
	#[inline]
	fn from(ast: Ast) -> Self {
		Self::Ast(Box::new(ast))
	}
}

impl ops::Add<Value> for Value {
	type Output = Self;

	fn add(self, rhs: Self) -> Self {
		if let Self::String(string) = self {
			Self::from(string.to_owned() + &rhs.as_string())
		} else {
			Self::from(self.as_number() + rhs.as_number())
		}
	}
}

impl ops::Sub<Value> for Value {
	type Output = Self;

	fn sub(self, rhs: Self) -> Self {
		Self::from(self.as_number() - rhs.as_number())
	}
}

impl ops::Mul<Value> for Value {
	type Output = Self;

	fn mul(self, rhs: Self) -> Self {
		if let Self::String(string) = self {
			let amnt = rhs.as_number();

			if amnt <= 0 || string.is_empty() {
				Self::from(String::default())
			} else if amnt > usize::MAX as Integer {
				panic!("Too much memory to be allocated!");
			} else {
				Self::from(string.repeat(amnt as usize))
			}
		} else {
			Self::from(self.as_number() * rhs.as_number())
		}
	}
}

impl ops::Div<Value> for Value {
	type Output = Self;

	fn div(self, rhs: Self) -> Self {
		let lhs = self.as_number();
		let rhs = rhs.as_number();

		if rhs == 0 {
			die!("division by zero encountered.");
		}

		Self::from(lhs / rhs)
	}
}

impl ops::Rem<Value> for Value {
	type Output = Self;

	fn rem(self, rhs: Self) -> Self {
		let lhs = self.as_number();
		let rhs = rhs.as_number();

		if rhs == 0 {
			die!("modulo by zero encountered.");
		}

		Self::from(lhs % rhs)
	}
}

impl Value {
	pub fn pow(self, rhs: Self) -> Self {
		Self::from(self.as_number().pow(rhs.as_number() as u32))
	}

	fn cmp(&self, rhs: &Self) -> std::cmp::Ordering {
		match self {
			Self::Null => die!("cannot compare null."),
			Self::Boolean(boolean) => boolean.cmp(&rhs.as_boolean()),
			Self::Number(integer) => integer.cmp(&rhs.as_number()),
			Self::String(string) => string.as_str().cmp(&rhs.as_string()),
			Self::Ast(ast) => ast.run().cmp(rhs)
		}
	}

	// note we do this because our `==` and comparison functions don't have the same
	// comparison semantics.
	pub fn lth(&self, rhs: &Self) -> bool {
		self.cmp(rhs) == std::cmp::Ordering::Less
	}

	// note we do this because our `==` and comparison functions don't have the same
	// comparison semantics.
	pub fn gth(&self, rhs: &Self) -> bool {
		self.cmp(rhs) == std::cmp::Ordering::Greater
	}

	pub fn run(&self) -> Value {
		if let Self::Ast(ast) = self {
			ast.run()
		} else {
			self.clone()
		}
	}
}
