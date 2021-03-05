use crate::{Function, Number, RcStr, RuntimeError};
use std::fmt::{self, Debug, Formatter};
use std::rc::Rc;
use std::convert::TryFrom;

#[derive(Clone)]
pub enum Value {
	Null,
	Boolean(bool),
	Number(Number),
	String(RcStr),
	Variable(String),
	Function(Function, Rc<[Value]>)
}

impl Default for Value {
	fn default() -> Self {
		Self::Null
	}
}

impl Eq for Value {}
impl PartialEq for Value {
	fn eq(&self, rhs: &Self) -> bool {
		match (self, rhs) {
			(Self::Null, Self::Null) => true,
			(Self::Boolean(lbool), Self::Boolean(rbool)) => lbool == rbool,
			(Self::Number(lnum), Self::Number(rnum)) => lnum == rnum,
			(Self::String(lstr), Self::String(rstr)) => lstr == rstr,
			(Self::Variable(lvar), Self::Variable(rvar)) => lvar == rvar,
			(Self::Function(lfunc, largs), Self::Function(rfunc, rargs)) =>
				lfunc == rfunc && Rc::ptr_eq(&largs, &rargs),
			_ => false
		}
	}
}

impl Debug for Value {
	// note we need the custom impl becuase `Null()` and `Identifier(...)` is required by the knight spec.
	fn fmt(&self, f: &mut Formatter) -> fmt::Result {
		match self {
			Self::Null => write!(f, "Null()"),
			Self::Boolean(boolean) => write!(f, "Boolean({})", boolean),
			Self::Number(number) => write!(f, "Number({})", number),
			Self::String(string) => write!(f, "String({})", string),
			Self::Variable(identifier) => write!(f, "Identifier({})", identifier),
			Self::Function(function, args) => write!(f, "Function({}, {:?})", function.name(), args),
		}
	}
}

impl From<bool> for Value {
	fn from(boolean: bool) -> Self {
		Self::Boolean(boolean)
	}
}

impl From<Number> for Value {
	fn from(number: Number) -> Self {
		Self::Number(number)
	}
}

impl From<String> for Value {
	fn from(string: String) -> Self {
		Self::String(string.into())
	}
}

impl From<RcStr> for Value {
	fn from(string: RcStr) -> Self {
		Self::String(string)
	}
}


impl TryFrom<&Value> for bool {
	type Error = RuntimeError;

	fn try_from(value: &Value) -> Result<Self, RuntimeError> {
		match value {
			Value::Null => Ok(false),
			Value::Boolean(boolean) => Ok(*boolean),
			Value::Number(number) => Ok(*number != 0),
			Value::String(string) => Ok(!string.is_empty()),
			_ => value.run().and_then(|value| TryFrom::try_from(&value))
		}
	}
}

impl TryFrom<&Value> for RcStr {
	type Error = RuntimeError;

	fn try_from(value: &Value) -> Result<Self, RuntimeError> {
		match value {
			Value::Null => Ok(unsafe { RcStr::new_literal_unchecked(b"null") }),
			Value::Boolean(true) => Ok(unsafe { RcStr::new_literal_unchecked(b"true") }),
			Value::Boolean(false) => Ok(unsafe { RcStr::new_literal_unchecked(b"false") }),
			Value::Number(0) => Ok(unsafe { RcStr::new_literal_unchecked(b"0") }),
			Value::Number(number) => Ok(RcStr::new_shared(number)),
			Value::String(string) => Ok(string.clone()),
			_ => value.run().and_then(|value| TryFrom::try_from(&value))
		}
	}
}

impl TryFrom<&Value> for Number {
	type Error = RuntimeError;

	fn try_from(value: &Value) -> Result<Self, RuntimeError> {
		match value {
			Value::Null => Ok(0),
			Value::Boolean(false) => Ok(0),
			Value::Boolean(true) => Ok(1),
			Value::Number(number) => Ok(*number),
			Value::String(string) => Ok({
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
	const fn typename(&self) -> &'static str {
		match self {
			Self::Null => "Null",
			Self::Boolean(_) => "Boolean",
			Self::Number(_) => "Number",
			Self::String(_) => "String",
			Self::Variable(_) => "Variable",
			Self::Function(_, _) => "Function",
		}
	}

	pub fn run(&self) -> Result<Self, RuntimeError> {
		match self {
			Self::Null => Ok(Self::Null),
			Self::Boolean(boolean) => Ok(Self::Boolean(*boolean)),
			Self::Number(number) => Ok(Self::Number(*number)),
			Self::String(rcstr) => Ok(Self::String(rcstr.clone())),
			Self::Variable(ident) => crate::env::get(ident),
			Self::Function(func, args) => (func.func())(&args),
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
		match self {
			Self::Number(lhs) =>
				rhs.to_number()
					.map(|rhs| lhs + rhs)
					.map(Self::Number),

			Self::String(lhs) =>
				rhs.to_rcstr()
					.map(|rhs| lhs.to_string() + &rhs)
					.map(From::from)
					.map(Self::String),

			_ => Err(RuntimeError::InvalidOperand { func: '+', operand: self.typename() })
		}
	}

	pub fn try_sub(&self, rhs: &Self) -> Result<Self, RuntimeError> {
		match self {
			Self::Number(lhs) =>
				rhs.to_number()
					.map(|rhs| lhs - rhs)
					.map(Self::Number),

			_ => Err(RuntimeError::InvalidOperand { func: '-', operand: self.typename() })
		}
	}

	pub fn try_mul(&self, rhs: &Self) -> Result<Self, RuntimeError> {
		match self {
			Self::Number(lhs) =>
				rhs.to_number()
					.map(|rhs| lhs * rhs)
					.map(Self::Number),

			Self::String(lhs) =>
				rhs.to_number()
					.map(|rhs| (0..rhs).map(|_| lhs.as_str()).collect::<String>())
					.map(From::from)
					.map(Self::String),

			_ => Err(RuntimeError::InvalidOperand { func: '*', operand: self.typename() })
		}
	}

	pub fn try_div(&self, rhs: &Self) -> Result<Self, RuntimeError> {
		let lhs = 
			if let Self::Number(lhs) =self {
				lhs
			} else {
				return Err(RuntimeError::InvalidOperand { func: '/', operand: self.typename() });
			};

		let rhs = rhs.to_number()?;

		if rhs == 0 {
			Err(RuntimeError::DivisionByZero { modulo: false })
		} else {
			Ok(Self::Number(lhs / rhs))
		}
	}

	pub fn try_rem(&self, rhs: &Self) -> Result<Self, RuntimeError> {
		let lhs = 
			if let Self::Number(lhs) =self {
				lhs
			} else {
				return Err(RuntimeError::InvalidOperand { func: '%', operand: self.typename() });
			};

		let rhs = rhs.to_number()?;

		if rhs == 0 {
			Err(RuntimeError::DivisionByZero { modulo: true })
		} else {
			Ok(Self::Number(lhs % rhs))
		}
	}

	pub fn try_pow(&self, rhs: &Self) -> Result<Self, RuntimeError> {
		let base = 
			if let Self::Number(lhs) = self {
				*lhs
			} else {
				return Err(RuntimeError::InvalidOperand { func: '^', operand: self.typename() });
			};

		let exponent = Number::try_from(rhs)?;

		Ok(Self::Number(
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
		))
	}

	pub fn try_lth(&self, rhs: &Self) -> Result<bool, RuntimeError> {
		match self {
			Self::Number(lhs) => rhs.to_number().map(|rhs| *lhs < rhs),
			Self::Boolean(lhs) => rhs.to_boolean().map(|rhs| !lhs && rhs),
			Self::String(lhs) => rhs.to_rcstr().map(|rhs| lhs.as_str() < rhs.as_str()),
			_ => Err(RuntimeError::InvalidOperand { func: '<', operand: self.typename() })
		}
	}

	pub fn try_gth(&self, rhs: &Self) -> Result<bool, RuntimeError> {
		match self {
			Self::Number(lhs) => rhs.to_number().map(|rhs| *lhs > rhs),
			Self::Boolean(lhs) => rhs.to_boolean().map(|rhs| *lhs && !rhs),
			Self::String(lhs) => rhs.to_rcstr().map(|rhs| lhs.as_str() > rhs.as_str()),
			_ => Err(RuntimeError::InvalidOperand { func: '>', operand: self.typename() })
		}
	}

	pub fn try_eql(&self, rhs: &Self) -> Result<bool, RuntimeError> {
		Ok(self == rhs)
	}
}
