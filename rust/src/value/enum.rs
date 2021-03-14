use crate::{Function, Number, RcStr, RuntimeError};
use std::fmt::{self, Debug, Formatter};
use std::rc::Rc;
use std::convert::{TryFrom, TryInto};

#[derive(Clone)]
pub struct Value(Inner);

#[derive(Clone)]
enum Inner {
	Null,
	Boolean(bool),
	Number(Number),
	String(RcStr),
	Variable(String),
	Function(Function, Rc<[Value]>)
}

impl Default for Value {
	fn default() -> Self {
		Self(Inner::Null)
	}
}

impl Eq for Value {}
impl PartialEq for Value {
	fn eq(&self, rhs: &Self) -> bool {
		match (&self.0, &rhs.0) {
			(Inner::Null, Inner::Null) => true,
			(Inner::Boolean(lbool), Inner::Boolean(rbool)) => lbool == rbool,
			(Inner::Number(lnum), Inner::Number(rnum)) => lnum == rnum,
			(Inner::String(lstr), Inner::String(rstr)) => lstr == rstr,
			(Inner::Variable(lvar), Inner::Variable(rvar)) => lvar == rvar,
			(Inner::Function(lfunc, largs), Inner::Function(rfunc, rargs)) =>
				lfunc == rfunc && Rc::ptr_eq(&largs, &rargs),
			_ => false
		}
	}
}

impl Debug for Value {
	// note we need the custom impl becuase `Null()` and `Identifier(...)` is required by the knight spec.
	fn fmt(&self, f: &mut Formatter) -> fmt::Result {
		match &self.0 {
			Inner::Null => write!(f, "Null()"),
			Inner::Boolean(boolean) => write!(f, "Boolean({})", boolean),
			Inner::Number(number) => write!(f, "Number({})", number),
			Inner::String(string) => write!(f, "String({})", string),
			Inner::Variable(identifier) => write!(f, "Identifier({})", identifier),
			Inner::Function(function, args) => write!(f, "Function({}, {:?})", function.name(), args),
		}
	}
}

impl From<bool> for Value {
	fn from(boolean: bool) -> Self {
		Self(Inner::Boolean(boolean))
	}
}

impl From<Number> for Value {
	fn from(number: Number) -> Self {
		Self(Inner::Number(number))
	}
}

impl TryFrom<String> for Value {
	type Error = crate::rcstr::InvalidString;

	fn try_from(string: String) -> Result<Self, Self::Error> {
		string.try_into().map(Inner::String).map(Self)
	}
}

impl From<RcStr> for Value {
	fn from(string: RcStr) -> Self {
		Self(Inner::String(string))
	}
}


impl TryFrom<&Value> for bool {
	type Error = RuntimeError;

	fn try_from(value: &Value) -> Result<Self, Self::Error> {
		match &value.0 {
			Inner::Null => Ok(false),
			Inner::Boolean(boolean) => Ok(*boolean),
			Inner::Number(number) => Ok(*number != 0),
			Inner::String(string) => Ok(!string.is_empty()),
			_ => value.run().and_then(|value| TryFrom::try_from(&value))
		}
	}
}

impl TryFrom<&Value> for RcStr {
	type Error = RuntimeError;

	fn try_from(value: &Value) -> Result<Self, Self::Error> {
		match &value.0 {
			Inner::Null => RcStr::new_literal(b"null").map_err(From::from),
			Inner::Boolean(true) => RcStr::new_literal(b"true").map_err(From::from),
			Inner::Boolean(false) => RcStr::new_literal(b"false").map_err(From::from),
			Inner::Number(0) => RcStr::new_literal(b"0").map_err(From::from),
			Inner::Number(number) => RcStr::new_shared(number).map_err(From::from),
			Inner::String(string) => Ok(string.clone()),
			_ => value.run().and_then(|value| TryFrom::try_from(&value))
		}
	}
}

impl TryFrom<&Value> for Number {
	type Error = RuntimeError;

	fn try_from(value: &Value) -> Result<Self, Self::Error> {
		match &value.0 {
			Inner::Null => Ok(0),
			Inner::Boolean(false) => Ok(0),
			Inner::Boolean(true) => Ok(1),
			Inner::Number(number) => Ok(*number),
			Inner::String(string) => Ok({
				let mut string = string.trim();
				let is_negative = string.chars().nth(0) == Some('-');

				if is_negative || string.chars().nth(0) == Some('+') {
					string = string.get(1..).unwrap();
				}

				(if is_negative { -1 } else { 1 }) * match string.find(|c: char| !c.is_ascii_digit()) {
					Some(0) => 0,
					Some(idx) => string.get(..idx).unwrap().parse().unwrap(),
					None if string.is_empty() => 0,
					None => string.parse().unwrap()
				}
			}),
			_ => value.run().and_then(|value| TryFrom::try_from(&value))
		}
	}
}

impl Value {
	pub fn new_variable(ident: String) -> Self {
		Self(Inner::Variable(ident))
	}

	pub fn new_function(func: Function, args: impl Into<Box<[Value]>>) -> Self {
		Self(Inner::Function(func, args.into().into()))
	}

	const fn typename(&self) -> &'static str {
		match self.0 {
			Inner::Null => "Null",
			Inner::Boolean(_) => "Boolean",
			Inner::Number(_) => "Number",
			Inner::String(_) => "String",
			Inner::Variable(_) => "Variable",
			Inner::Function(_, _) => "Function",
		}
	}

	pub fn run(&self) -> Result<Self, RuntimeError> {
		match &self.0 {
			Inner::Null => Ok(Self(Inner::Null)),
			Inner::Boolean(boolean) => Ok(Self(Inner::Boolean(*boolean))),
			Inner::Number(number) => Ok(Self(Inner::Number(*number))),
			Inner::String(rcstr) => Ok(Self(Inner::String(rcstr.clone()))),
			Inner::Variable(ident) => crate::env::get(ident),
			Inner::Function(func, args) => (func.func())(&args),
		}
	}

	pub fn to_boolean(&self) -> Result<bool, RuntimeError> {
		TryFrom::try_from(self)
	}

	pub fn to_number(&self) -> Result<Number, RuntimeError> {
		TryFrom::try_from(self)
	}

	pub fn to_rcstr(&self) -> Result<RcStr, RuntimeError> {
		TryFrom::try_from(self)
	}
}

impl Value {
	pub fn try_add(&self, rhs: &Self) -> Result<Self, RuntimeError> {
		match &self.0 {
			Inner::Number(lhs) =>
				rhs.to_number()
					.map(|rhs| lhs + rhs)
					.map(Self::from),

			Inner::String(lhs) =>
				rhs.to_rcstr()
					.map(|rhs| lhs.to_string() + &rhs)
					.map(String::from)?
					.try_into()
					.map_err(From::from),

			_ => Err(runtime_error!(InvalidOperand { func: '+', operand: self.typename() }))
		}
	}

	pub fn try_sub(&self, rhs: &Self) -> Result<Self, RuntimeError> {
		if let Inner::Number(lhs) = self.0 {
			rhs.to_number().map(|rhs| lhs - rhs).map(Self::from)
		} else {
			Err(runtime_error!(InvalidOperand { func: '-', operand: self.typename() }))
		}
	}

	pub fn try_mul(&self, rhs: &Self) -> Result<Self, RuntimeError> {
		match &self.0 {
			Inner::Number(lhs) =>
				rhs.to_number()
					.map(|rhs| lhs * rhs)
					.map(Self::from),

			Inner::String(lhs) =>
				rhs.to_number()
					.map(|rhs| (0..rhs).map(|_| lhs.as_str()).collect::<String>())
					.map(String::from)?
					.try_into()
					.map_err(From::from),

			_ => Err(runtime_error!(InvalidOperand { func: '*', operand: self.typename() }))
		}
	}

	pub fn try_div(&self, rhs: &Self) -> Result<Self, RuntimeError> {
		let lhs = 
			if let Inner::Number(lhs) = self.0 {
				lhs
			} else {
				return Err(runtime_error!(InvalidOperand { func: '/', operand: self.typename() }));
			};

		let rhs = rhs.to_number()?;

		if rhs == 0 {
			Err(runtime_error!(DivisionByZero { modulo: false }))
		} else {
			Ok(Self(Inner::Number(lhs / rhs)))
		}
	}

	pub fn try_rem(&self, rhs: &Self) -> Result<Self, RuntimeError> {
		let lhs = 
			if let Inner::Number(lhs) = self.0 {
				lhs
			} else {
				return Err(runtime_error!(InvalidOperand { func: '%', operand: self.typename() }));
			};

		let rhs = rhs.to_number()?;

		if rhs == 0 {
			Err(runtime_error!(DivisionByZero { modulo: true }))
		} else {
			Ok(Self(Inner::Number(lhs % rhs)))
		}
	}

	pub fn try_pow(&self, rhs: &Self) -> Result<Self, RuntimeError> {
		let base = 
			if let Inner::Number(lhs) = self.0 {
				lhs
			} else {
				return Err(runtime_error!(InvalidOperand { func: '^', operand: self.typename() }));
			};

		let exponent = Number::try_from(rhs)?;

		Ok(Self(Inner::Number(
			if base == 1 {
				1
			} else if base == -1 {
				if exponent & 1 == 1 {
					-1
				} else {
					1
				}
			} else {
				match exponent {
					1 => base,
					0 => 1,
					_ if exponent < 0 => 0,
					_ => base.pow(exponent as u32)
				}
			}
		)))
	}

	pub fn try_lth(&self, rhs: &Self) -> Result<bool, RuntimeError> {
		match &self.0 {
			Inner::Number(lhs) => rhs.to_number().map(|rhs| *lhs < rhs),
			Inner::Boolean(lhs) => rhs.to_boolean().map(|rhs| !lhs && rhs),
			Inner::String(lhs) => rhs.to_rcstr().map(|rhs| lhs.as_str() < rhs.as_str()),
			_ => Err(runtime_error!(InvalidOperand { func: '<', operand: self.typename() }))
		}
	}

	pub fn try_gth(&self, rhs: &Self) -> Result<bool, RuntimeError> {
		match &self.0 {
			Inner::Number(lhs) => rhs.to_number().map(|rhs| *lhs > rhs),
			Inner::Boolean(lhs) => rhs.to_boolean().map(|rhs| *lhs && !rhs),
			Inner::String(lhs) => rhs.to_rcstr().map(|rhs| lhs.as_str() > rhs.as_str()),
			_ => Err(runtime_error!(InvalidOperand { func: '>', operand: self.typename() }))
		}
	}

	pub fn try_eql(&self, rhs: &Self) -> Result<bool, RuntimeError> {
		Ok(self == rhs)
	}

	pub fn try_assign(&self, value: Value) -> Result<(), RuntimeError> {
		if let Inner::Variable(ref ident) = self.0 {
			crate::env::insert(ident, value);
			Ok(())
		} else {
			Err(runtime_error!(InvalidOperand { func: '=', operand: self.typename() }))
		}
	}
}
