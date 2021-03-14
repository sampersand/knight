#![cfg_attr(feature="reckless", allow(unused))]

#[cfg(all(feature="pretty-errors", any(feature="reckless", feature="fatal-errors")))]
compile_error!("'pretty-errors' cannot be enabled with either 'reckless' or 'fatal-errors'!");

macro_rules! unreachable_unchecked {
	() => {
		if cfg!(debug_assertions) {
			unreachable!();
		} else {
			std::hint::unreachable_unchecked();
		}
	};
}

#[cfg(not(feature = "reckless"))]
macro_rules! parse_error {
	($variant:ident $($rest:tt)*) => {
		Err(ParseError::from($crate::error::ParseErrorKind::$variant $($rest)*))
	};
}

#[cfg(feature = "reckless")]
macro_rules! parse_error {
	($($_tt:tt)*) => { unsafe { unreachable_unchecked!() } }
}

#[cfg(not(feature = "reckless"))]
macro_rules! runtime_error {
	($variant:ident $($rest:tt)*) => {
		Err(RuntimeError::from($crate::error::RuntimeErrorKind::$variant $($rest)*))
	};
}

#[cfg(feature = "reckless")]
macro_rules! runtime_error {
	($($_tt:tt)*) => { unsafe { unreachable_unchecked!() } }
}



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