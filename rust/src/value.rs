use crate::{Function, Number, RcString, Variable, RuntimeError, Environment};
use std::fmt::{self, Debug, Formatter};
use std::rc::Rc;
use std::convert::TryFrom;

#[derive(Clone)]
pub enum Value {
	Null,
	Boolean(bool),
	Number(Number),
	String(RcString),
	Variable(Variable),
	Function(Function, Rc<[Value]>)
}

impl Default for Value {
	#[inline]
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
			(Self::Function(lfunc, largs), Self::Function(rfunc, rargs)) => lfunc == rfunc && Rc::ptr_eq(&largs, &rargs),
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
			Self::Variable(variable) => write!(f, "Variable({})", variable.name()),
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

impl From<RcString> for Value {
	fn from(string: RcString) -> Self {
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

impl TryFrom<&Value> for RcString {
	type Error = RuntimeError;

	fn try_from(value: &Value) -> Result<Self, RuntimeError> {
		use once_cell::sync::OnceCell;

		static NULL: OnceCell<RcString> = OnceCell::new();
		static TRUE: OnceCell<RcString> = OnceCell::new();
		static FALSE: OnceCell<RcString> = OnceCell::new();
		static ZERO: OnceCell<RcString> = OnceCell::new();
		static ONE: OnceCell<RcString> = OnceCell::new();

		

		match value {
			Value::Null => Ok(NULL.get_or_init(|| unsafe { RcString::new_unchecked("null") }).clone()),
			Value::Boolean(true) => Ok(TRUE.get_or_init(|| unsafe { RcString::new_unchecked("true") }).clone()),
			Value::Boolean(false) => Ok(FALSE.get_or_init(|| unsafe { RcString::new_unchecked("false") }).clone()),
			Value::Number(0) => Ok(ZERO.get_or_init(|| unsafe { RcString::new_unchecked("0") }).clone()),
			Value::Number(1) => Ok(ONE.get_or_init(|| unsafe { RcString::new_unchecked("1") }).clone()),
			Value::Number(number) => Ok(RcString::new(number.to_string()).unwrap()), // all numbers should be valid strings
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
			Value::String(string) => {
				let mut chars = string.trim().bytes();
				let mut sign = 1;
				let mut number = 0 as Number;

				match chars.next() {
					Some(b'-') => sign = -1,
					Some(b'+') => { /* do nothing */ },
					Some(digit @ b'0'..=b'9') => number = (digit - b'0') as Number,
					_ => return Ok(0)
				};

				while let Some(digit @ b'0'..=b'9') = chars.next() {
					number *= 10;
					number += (digit - b'0') as Number;
				}

				Ok(sign * number)
			},
			_ => Err(RuntimeError::UndefinedConversion { into: "bool", kind: value.typename() })
		}
	}
}

impl Value {
	pub fn run(&self, env: &mut Environment) -> Result<Self, RuntimeError> {
		match self {
			Self::Null => Ok(Self::Null),
			Self::Boolean(boolean) => Ok(Self::Boolean(*boolean)),
			Self::Number(number) => Ok(Self::Number(*number)),
			Self::String(rcstring) => Ok(Self::String(rcstring.clone())),
			Self::Variable(variable) => variable.fetch()
				.ok_or_else(|| RuntimeError::UnknownIdentifier { identifier: variable.name().into() }),
			Self::Function(func, args) => func.run(&args, env),
		}
	}

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

	pub fn to_boolean(&self) -> Result<bool, RuntimeError> {
		TryFrom::try_from(self)
	}

	pub fn to_number(&self) -> Result<Number, RuntimeError> {
		TryFrom::try_from(self)
	}

	pub fn to_rcstring(&self) -> Result<RcString, RuntimeError> {
		TryFrom::try_from(self)
	}
}
