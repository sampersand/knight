// #![warn(missing_docs, missing_doc_code_examples)]
#![allow(clippy::tabs_in_doc_comments)]
#![warn(/*, missing_doc_code_examples, missing_docs*/)]

pub mod function;
pub mod rcstring;
mod value;
mod error;
mod stream;
pub mod environment;

/// The number type within Knight.
pub type Number = i64;

#[doc(inline)]
pub use rcstring::RcString;

#[doc(inline)]
pub use function::Function;

pub use environment::{Environment, Variable};
pub use value::Value;
pub use error::{ParseError, RuntimeError};

/// Runs the given string as Knight code, returning the result of its execution.
pub fn run_str<S: AsRef<str>>(input: S, env: &mut Environment<'_, '_, '_>) -> Result<Value, RuntimeError> {
	run(input.as_ref().chars(), env)
}

/// Parses a [`Value`] from the given iterator and then runs the value.
pub fn run<I>(input: I, env: &mut Environment<'_, '_, '_>) -> Result<Value, RuntimeError>
where
	I: IntoIterator<Item=char>
{
	Value::parse(input, env)?.run(env)
}