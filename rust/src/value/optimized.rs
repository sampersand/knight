use crate::{RcStr, env::Variable, RuntimeError, rcstr::InvalidString};
use static_assertions::{const_assert_eq, const_assert};
use std::fmt::{self, Debug, Formatter};
use std::convert::TryFrom;

pub struct Value(u64);

const FALSE: Value = Value(0b0000);
const NULL: Value  = Value(0b0010);
const TRUE: Value  = Value(0b0100);

const NUMBER_BIT: u64    = 0b0001;
const STRING_MASK: u64   = 0b0000;
const VARIABLE_MASK: u64 = 0b0010;
const FUNCTION_MASK: u64 = 0b0100;
const MASK: u64 = NUMBER_BIT | STRING_MASK | VARIABLE_MASK | FUNCTION_MASK;

impl Value {
	const fn mask(&self) -> u64 { self.0 & MASK }
	const fn unmask(&self) -> u64 { self.0 & !MASK }

	pub const fn is_null(&self)    -> bool { self.0 == NULL.0 }
	pub const fn is_boolean(&self) -> bool { self.0 == TRUE.0 || self.0 == FALSE.0 }
	pub const fn is_number(&self)  -> bool { self.0 & NUMBER_BIT == NUMBER_BIT }
	pub const fn is_string(&self)  -> bool { self.0 != STRING_MASK && self.mask() == STRING_MASK }
	pub const fn is_variable(&self) -> bool { self.0 != VARIABLE_MASK && self.mask() == VARIABLE_MASK }
	pub const fn is_function(&self) -> bool { self.0 != FUNCTION_MASK && self.mask() == FUNCTION_MASK }
	const fn is_literal(&self) -> bool { self.0 <= TRUE.0 || self.is_number() }

	pub fn as_boolean(&self) -> Option<bool> {
		if self.is_boolean() {
			Some(unsafe { self.as_boolean_unchecked() })
		} else {
			None
		}
	}

	pub unsafe fn as_boolean_unchecked(&self) -> bool {
		debug_assert!(self.is_boolean());

		self.0 == TRUE.0
	}

	pub fn as_number(&self) -> Option<i64> {
		if self.is_number() {
			Some(unsafe { self.as_number_unchecked() })
		} else {
			None
		}
	}

	pub unsafe fn as_number_unchecked(&self) -> i64 {
		debug_assert!(self.is_number());

		(self.0 as i64) >> 1
	}

	pub fn as_string(&self) -> Option<&RcStr> {
		if self.is_number() {
			Some(unsafe { self.as_string_unchecked() })
		} else {
			None
		}
	}

	pub unsafe fn as_string_unchecked(&self) -> &RcStr {
		debug_assert!(self.is_string());

		// self.unmask() as *const u8
		todo!();
	}

	pub fn as_variable(&self) -> Option<&Variable> {
		if self.is_number() {
			Some(unsafe { self.as_variable_unchecked() })
		} else {
			None
		}
	}

	pub unsafe fn as_variable_unchecked(&self) -> &Variable {
		debug_assert!(self.is_variable());

		// self.unmask() as *const u8
		todo!();
	}
}

impl Default for Value {
	fn default() -> Self {
		NULL
	}
}

impl Eq for Value {}
impl PartialEq for Value {
	fn eq(&self, rhs: &Self) -> bool {
		if self.0 == rhs.0 {
			return true;
		}

		if let Some(string) = self.as_string() {
			rhs.as_string().map_or(false, |rstr| string.as_str() == rstr.as_str())
		} else {
			false
		}
	}
}

impl Clone for Value {
	fn clone(&self) -> Self {
		if self.is_literal() || self.is_variable() {
			Self(self.0)
		} else if let Some(string) = self.as_string() {
			Self(string.clone().into_raw() as u64)
		} else {
			debug_assert!(self.is_function());

			todo!();
		}
	}
}

impl Debug for Value {
	fn fmt(&self, f: &mut Formatter) -> fmt::Result {
		if let Some(boolean) = self.as_boolean() {
			write!(f, "Boolean({})", boolean)
		} else if self.is_null() {
			write!(f, "Null()")
		} else if let Some(number) = self.as_number() {
			write!(f, "Number({})", number)
		} else if let Some(variable) = self.as_variable() {
			write!(f, "Identifier({})", variable.identifier())
		} else {
			debug_assert!(self.is_function());

			todo!()
		}
	}
}

impl From<bool> for Value {
	#[inline]
	fn from(boolean: bool) -> Self {
		const_assert_eq!((false as u64) << 2, FALSE.0);
		const_assert_eq!((true as u64) << 2, TRUE.0);

		Self((boolean as u64) << 2)
	}
}

// TODO: make this consistent with the enum version
impl From<i64> for Value {
	fn from(num: i64) -> Value {
		if !cfg!(feature = "reckless") || cfg!(debug_assertions) {
			assert_eq!((num >> 1) << 1, num);
		}

		Self((num >> 1) as u64)
	}
}

impl TryFrom<String> for Value {
	type Error = InvalidString;

	fn try_from(string: String) -> Result<Self, Self::Error> {
		Ok(RcStr::try_from(string)?.into())
	}
}

impl From<RcStr> for Value {
	fn from(string: RcStr) -> Self {
		let ptr = RcStr::into_raw(string);

		debug_assert_ne!((ptr as u64) & MASK, STRING_MASK);

		Self((ptr as u64) | STRING_MASK)
	}
}

impl TryFrom<&Value> for bool {
	type Error = RuntimeError;
	fn try_from(value: &Value) -> Result<Self, Self::Error> {
		const_assert!(FALSE.0 <= NULL.0);
		const_assert!(((0 << 1) | NUMBER_BIT) <= NULL.0);
		const_assert!(NULL.0 <= NULL.0);
		const_assert!(((1 << 1) | NUMBER_BIT) > NULL.0);
		const_assert!(TRUE.0 > NULL.0);

		if value.0 <= NULL.0 {
			Ok(false)
		} else if value.0 == TRUE.0 || value.is_number() {
			Ok(true)
		} else if let Some(string) = value.as_string() {
			Ok(!string.as_ref().is_empty())
		} else {
			// value.run()?.try_into().map_err(From::from)
			todo!()
		}
	}
}

impl Value {
	pub fn run(&self) -> Result<Self, RuntimeError> {
		if self.is_literal() {
			Ok(Self(self.0))
		} else if let Some(string) = self.as_string() {
			Ok(string.clone().into())
		} else if let Some(variable) = self.as_variable() {
			variable.fetch()
		} else {
			assert!(self.is_function());

			todo!();
		}
	}
}

// impl TryFrom<&Value> for RcStr {
// 	type Error = RuntimeError;

// 	fn try_from(value: &Value) -> Result<Self, Self::Error> {
// 		match &value.0 {
// 			Inner::Null => RcStr::new_literal(b"null").map_err(From::from),
// 			Inner::Boolean(true) => RcStr::new_literal(b"true").map_err(From::from),
// 			Inner::Boolean(false) => RcStr::new_literal(b"false").map_err(From::from),
// 			Inner::Number(0) => RcStr::new_literal(b"0").map_err(From::from),
// 			Inner::Number(number) => RcStr::new_shared(number).map_err(From::from),
// 			Inner::String(string) => Ok(string.clone()),
// 			_ => value.run().and_then(|value| TryFrom::try_from(&value))
// 		}
// 	}
// }

// impl TryFrom<&Value> for Number {
// 	type Error = RuntimeError;

// 	fn try_from(value: &Value) -> Result<Self, Self::Error> {
// 		match &value.0 {
// 			Inner::Null => Ok(0),
// 			Inner::Boolean(false) => Ok(0),
// 			Inner::Boolean(true) => Ok(1),
// 			Inner::Number(number) => Ok(*number),
// 			Inner::String(string) => Ok({
// 				let mut string = string.trim();
// 				let is_negative = string.chars().nth(0) == Some('-');

// 				if is_negative || string.chars().nth(0) == Some('+') {
// 					string = string.get(1..).unwrap();
// 				}

// 				(if is_negative { -1 } else { 1 }) * match string.find(|c: char| !c.is_ascii_digit()) {
// 					Some(0) => 0,
// 					Some(idx) => string.get(..idx).unwrap().parse().unwrap(),
// 					None if string.is_empty() => 0,
// 					None => string.parse().unwrap()
// 				}
// 			}),
// 			_ => value.run().and_then(|value| TryFrom::try_from(&value))
// 		}
// 	}
// }

// impl Value {
// 	pub fn new_variable(ident: String) -> Self {
// 		Self(Inner::Variable(ident))
// 	}

// 	pub fn new_function(func: Function, args: impl Into<Box<[Value]>>) -> Self {
// 		Self(Inner::Function(func, args.into().into()))
// 	}

// 	const fn typename(&self) -> &'static str {
// 		match self.0 {
// 			Inner::Null => "Null",
// 			Inner::Boolean(_) => "Boolean",
// 			Inner::Number(_) => "Number",
// 			Inner::String(_) => "String",
// 			Inner::Variable(_) => "Variable",
// 			Inner::Function(_, _) => "Function",
// 		}
// 	}

// 	pub fn run(&self) -> Result<Self, RuntimeError> {
// 		match &self.0 {
// 			Inner::Null => Ok(Self(Inner::Null)),
// 			Inner::Boolean(boolean) => Ok(Self(Inner::Boolean(*boolean))),
// 			Inner::Number(number) => Ok(Self(Inner::Number(*number))),
// 			Inner::String(rcstr) => Ok(Self(Inner::String(rcstr.clone()))),
// 			Inner::Variable(ident) => crate::env::get(ident),
// 			Inner::Function(func, args) => (func.func())(&args),
// 		}
// 	}

// 	pub fn to_boolean(&self) -> Result<bool, RuntimeError> {
// 		TryFrom::try_from(self)
// 	}

// 	pub fn to_number(&self) -> Result<Number, RuntimeError> {
// 		TryFrom::try_from(self)
// 	}

// 	pub fn to_rcstr(&self) -> Result<RcStr, RuntimeError> {
// 		TryFrom::try_from(self)
// 	}
// }

// impl Value {
// 	pub fn try_add(&self, rhs: &Self) -> Result<Self, RuntimeError> {
// 		match &self.0 {
// 			Inner::Number(lhs) =>
// 				rhs.to_number()
// 					.map(|rhs| lhs + rhs)
// 					.map(Self::from),

// 			Inner::String(lhs) =>
// 				rhs.to_rcstr()
// 					.map(|rhs| lhs.to_string() + &rhs)
// 					.map(String::from)?
// 					.try_into()
// 					.map_err(From::from),

// 			_ => Err(runtime_error!(InvalidOperand { func: '+', operand: self.typename() }))
// 		}
// 	}

// 	pub fn try_sub(&self, rhs: &Self) -> Result<Self, RuntimeError> {
// 		if let Inner::Number(lhs) = self.0 {
// 			rhs.to_number().map(|rhs| lhs - rhs).map(Self::from)
// 		} else {
// 			Err(runtime_error!(InvalidOperand { func: '-', operand: self.typename() }))
// 		}
// 	}

// 	pub fn try_mul(&self, rhs: &Self) -> Result<Self, RuntimeError> {
// 		match &self.0 {
// 			Inner::Number(lhs) =>
// 				rhs.to_number()
// 					.map(|rhs| lhs * rhs)
// 					.map(Self::from),

// 			Inner::String(lhs) =>
// 				rhs.to_number()
// 					.map(|rhs| (0..rhs).map(|_| lhs.as_str()).collect::<String>())
// 					.map(String::from)?
// 					.try_into()
// 					.map_err(From::from),

// 			_ => Err(runtime_error!(InvalidOperand { func: '*', operand: self.typename() }))
// 		}
// 	}

// 	pub fn try_div(&self, rhs: &Self) -> Result<Self, RuntimeError> {
// 		let lhs = 
// 			if let Inner::Number(lhs) = self.0 {
// 				lhs
// 			} else {
// 				return Err(runtime_error!(InvalidOperand { func: '/', operand: self.typename() }));
// 			};

// 		let rhs = rhs.to_number()?;

// 		if rhs == 0 {
// 			Err(runtime_error!(DivisionByZero { modulo: false }))
// 		} else {
// 			Ok(Self(Inner::Number(lhs / rhs)))
// 		}
// 	}

// 	pub fn try_rem(&self, rhs: &Self) -> Result<Self, RuntimeError> {
// 		let lhs = 
// 			if let Inner::Number(lhs) = self.0 {
// 				lhs
// 			} else {
// 				return Err(runtime_error!(InvalidOperand { func: '%', operand: self.typename() }));
// 			};

// 		let rhs = rhs.to_number()?;

// 		if rhs == 0 {
// 			Err(runtime_error!(DivisionByZero { modulo: true }))
// 		} else {
// 			Ok(Self(Inner::Number(lhs % rhs)))
// 		}
// 	}

// 	pub fn try_pow(&self, rhs: &Self) -> Result<Self, RuntimeError> {
// 		let base = 
// 			if let Inner::Number(lhs) = self.0 {
// 				lhs
// 			} else {
// 				return Err(runtime_error!(InvalidOperand { func: '^', operand: self.typename() }));
// 			};

// 		let exponent = Number::try_from(rhs)?;

// 		Ok(Self(Inner::Number(
// 			if base == 1 {
// 				1
// 			} else if base == -1 {
// 				if exponent & 1 == 1 {
// 					-1
// 				} else {
// 					1
// 				}
// 			} else {
// 				match exponent {
// 					1 => base,
// 					0 => 1,
// 					_ if exponent < 0 => 0,
// 					_ => base.pow(exponent as u32)
// 				}
// 			}
// 		)))
// 	}

// 	pub fn try_lth(&self, rhs: &Self) -> Result<bool, RuntimeError> {
// 		match &self.0 {
// 			Inner::Number(lhs) => rhs.to_number().map(|rhs| *lhs < rhs),
// 			Inner::Boolean(lhs) => rhs.to_boolean().map(|rhs| !lhs && rhs),
// 			Inner::String(lhs) => rhs.to_rcstr().map(|rhs| lhs.as_str() < rhs.as_str()),
// 			_ => Err(runtime_error!(InvalidOperand { func: '<', operand: self.typename() }))
// 		}
// 	}

// 	pub fn try_gth(&self, rhs: &Self) -> Result<bool, RuntimeError> {
// 		match &self.0 {
// 			Inner::Number(lhs) => rhs.to_number().map(|rhs| *lhs > rhs),
// 			Inner::Boolean(lhs) => rhs.to_boolean().map(|rhs| *lhs && !rhs),
// 			Inner::String(lhs) => rhs.to_rcstr().map(|rhs| lhs.as_str() > rhs.as_str()),
// 			_ => Err(runtime_error!(InvalidOperand { func: '>', operand: self.typename() }))
// 		}
// 	}

// 	pub fn try_eql(&self, rhs: &Self) -> Result<bool, RuntimeError> {
// 		Ok(self == rhs)
// 	}

// 	pub fn try_assign(&self, value: Value) -> Result<(), RuntimeError> {
// 		if let Inner::Variable(ref ident) = self.0 {
// 			crate::env::insert(ident, value);
// 			Ok(())
// 		} else {
// 			Err(runtime_error!(InvalidOperand { func: '=', operand: self.typename() }))
// 		}
// 	}
// }
