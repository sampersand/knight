#![cfg_attr(feature="reckless", allow(unused))]

#[macro_use]
extern crate cfg_if;

#[allow(unused)]
macro_rules! unreachable_unchecked {
	() => {
		if cfg!(debug_assertions) {
			unreachable!();
		} else {
			std::hint::unreachable_unchecked();
		}
	};
}

cfg_if! {
	if #[cfg(feature="reckless")] {
		macro_rules! parse_error {
			($($_tt:tt)*) => { unsafe { unreachable_unchecked!() } }
		}

		macro_rules! runtime_error {
			($($_tt:tt)*) => { unsafe { unreachable_unchecked!() } }
		}
	} else {
		macro_rules! parse_error {
			($variant:ident $($rest:tt)*) => { ParseError::from($crate::error::ParseErrorKind::$variant $($rest)*) };
		}

		macro_rules! runtime_error {
			($variant:ident $($rest:tt)*) => { RuntimeError::from($crate::error::RuntimeErrorKind::$variant $($rest)*) };
		}
	}
}

#[cfg_attr(feature="optimized-value", path="value/optimized.rs")]
#[cfg_attr(not(feature="optimized-value"), path="value/enum.rs")]
mod value;


mod error;
mod function;
pub mod rcstr;
mod stream;
pub mod env;


pub type Number = i64;
pub use rcstr::RcStr;
pub use value::Value;
pub use function::Function;
pub use error::{ParseError, RuntimeError};

pub fn run_str<S: AsRef<str>>(input: S) -> Result<Value, RuntimeError> {
	Value::parse_str(input)?.run()
}

pub fn run<I: Iterator<Item=char>>(input: I) -> Result<Value, RuntimeError> {
	Value::parse(input)?.run()
}