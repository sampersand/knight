use std::collections::HashMap;
use std::mem::MaybeUninit;
use crate::Value;

static mut ENVIRONMENT: MaybeUninit<HashMap<String, Value>> = MaybeUninit::uninit();


static mut INITIALIZED: bool = false;

pub fn initialize() {
	unsafe {
		if !INITIALIZED {
			INITIALIZED = true;
			ENVIRONMENT.as_mut_ptr().write(HashMap::new());
		}
	}
}

pub fn get(ident: &str) -> Option<&Value> {
	// SAFETY: Knight only ever runs in one thread, so no race conditions.
	//
	// additionally, we know that `ENVIRONMENT` is initialized because `initialize` is always
	// run before knight code is executed.
	unsafe {
		debug_assert!(INITIALIZED);

		(*ENVIRONMENT.as_ptr()).get(ident)
	}
}

pub fn set(ident: String, value: Value) {
	// SAFETY: Knight only ever runs in one thread, so no race conditions.
	//
	// additionally, we know that `ENVIRONMENT` is initialized because `initialize` is always
	// run before knight code is executed.
	unsafe {
		debug_assert!(INITIALIZED);

		(*ENVIRONMENT.as_mut_ptr()).insert(ident, value);
	}
}
