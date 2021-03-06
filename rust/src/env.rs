use crate::{Value, RuntimeError};
use std::collections::HashMap;
use once_cell::unsync::OnceCell;
use std::cell::RefCell;

static mut ENVIRONMENT: OnceCell<RefCell<HashMap<&'static str, Value>>> = OnceCell::new();

#[allow(unsafe_code)]
pub fn get(ident: &str) -> Result<Value, RuntimeError> {
	unsafe { &ENVIRONMENT }
		.get()
		.and_then(|env| env.borrow().get(ident).cloned())
		.ok_or_else(|| RuntimeError::UnknownIdentifier { identifier: ident.to_owned() })
}

#[allow(unsafe_code)]
pub fn insert(ident: &str, value: Value) {
	let mut env =
		unsafe { &ENVIRONMENT }
			.get_or_init(Default::default)
			.borrow_mut();

	if let Some(old) = env.get_mut(ident) {
		*old = value 
	} else {
		env.insert(Box::leak(ident.to_string().into_boxed_str()), value);
	}
}