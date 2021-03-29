#[macro_use]
extern crate cfg_if;

pub mod function;
pub mod rcstr;
mod value;
mod error;
mod stream;
pub mod env;

/// The number type within Knight.
pub type Number = i64;

#[doc(inline)]
pub use rcstr::RcStr;

#[doc(inline)]
pub use function::Function;

pub use env::{Environment, Variable};
pub use value::Value;
pub use error::{ParseError, RuntimeError};

/// Runs the given string as Knight code, returning the result of its execution.
pub fn run_str<S: AsRef<str>>(input: S, env: &Environment) -> Result<Value, RuntimeError> {
	run(input.as_ref().chars(), env)
}

/// Parses a [`Value`] from the given iterator and then runs the value.
pub fn run<I: IntoIterator<Item=char>>(input: I, env: &Environment) -> Result<Value, RuntimeError> {
	Value::parse(input, env)?.run(env)
}