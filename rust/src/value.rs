use crate::{Function, Number, RcStr, Variable, RuntimeError, Environment};
use std::fmt::{self, Debug, Formatter};
use std::rc::Rc;
use std::convert::TryFrom;

#[derive(Clone)]
pub enum Value {
	Null,
	Boolean(bool),
	Number(Number),
	String(RcStr),
	Variable(Variable),
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
			Self::Variable(variable) => write!(f, "Identifier({})", variable.name()),
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

impl From<RcStr> for Value {
	fn from(string: RcStr) -> Self {
		Self::String(string)
	}
}

impl From<Variable> for Value {
	fn from(variable: Variable) -> Self {
		Self::Variable(variable)
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
			_ => Err(RuntimeError::UndefinedConversion { into: "bool", kind: value.typename() })
		}
	}
}

impl TryFrom<&Value> for RcStr {
	type Error = RuntimeError;

	fn try_from(value: &Value) -> Result<Self, RuntimeError> {
		match value {
			Value::Null => Ok(unsafe { RcStr::new_unchecked("null") }),
			Value::Boolean(true) => Ok(unsafe { RcStr::new_unchecked("true") }),
			Value::Boolean(false) => Ok(unsafe { RcStr::new_unchecked("false") }),
			Value::Number(0) => Ok(unsafe { RcStr::new_unchecked("0") }),
			Value::Number(number) => Ok(RcStr::new(number.to_string()).unwrap()),
			Value::String(string) => Ok(string.clone()),
			_ => Err(RuntimeError::UndefinedConversion { into: "bool", kind: value.typename() })
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
			_ => Err(RuntimeError::UndefinedConversion { into: "bool", kind: value.typename() })
		}
	}
}

impl Value {
	pub const fn typename(&self) -> &'static str {
		match self {
			Self::Null => "Null",
			Self::Boolean(_) => "Boolean",
			Self::Number(_) => "Number",
			Self::String(_) => "String",
			Self::Variable(_) => "Variable",
			Self::Function(_, _) => "Function",
		}
	}

	pub fn run(&self, env: &mut Environment) -> Result<Self, RuntimeError> {
		match self {
			Self::Null => Ok(Self::Null),
			Self::Boolean(boolean) => Ok(Self::Boolean(*boolean)),
			Self::Number(number) => Ok(Self::Number(*number)),
			Self::String(rcstr) => Ok(Self::String(rcstr.clone())),
			Self::Variable(variable) => variable.fetch()
				.ok_or_else(|| RuntimeError::UnknownIdentifier { identifier: variable.name().into() }),
			Self::Function(func, args) => func.run(&args, env),
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
