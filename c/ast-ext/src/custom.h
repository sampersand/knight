#ifndef KN_CUSTOM_H
#define KN_CUSTOM_H
#ifdef KN_EXT_CUSTOM_TYPES

#include <stdlib.h> /* NULL, free */
#include "value.h"  /* kn_value, kn_number, kn_boolean, kn_string */

/*
 * The virtual table for custom types.
 *
 * There should generally only be one of these per type. Functions may be
 * omitted (by assigning them to `NULL`); if they are, their default
 * implementations will be used.
 *
 * Note that if `run` is omitted and not all of `to_number`, `to_boolean`, and
 * `to_string` are defined, an infinite loop will occur.
 */
struct kn_custom_vtable {
	/*
	 * Duplicates `data`, returning a new instance of it.
	 *
	 * The return will be passed to `free` in addition to `data`. This function
	 * is required and does not have a default value.
	 */
	void *(*clone)(void *data);

	/*
	 * Releases the resources associated with `data`.
	 *
	 * The default implementation simply calls the stdlib's `free`.
	 */
	void (*free)(void *data);

	/*
	 * Dumps debugging info for `data`.
	 *
	 * The default implementation writes `Custom(<data ptr>, <vtable ptr>)` to
	 * stdout.
	 */
	void (*dump)(void *data);

	/*
	 * Executes the given `data`, returning the value associated with it.
	 *
	 * The default implementation simply calls `clone` and duplicates it.
	 */
	kn_value (*run)(void *data);

	/*
	 * Converts the `data` to a number.
	 *
	 * The default implementation will call `run`, and then convert the result
	 * to a number.
	 */
	kn_number (*to_number)(void *data);

	/*
	 * Converts the `data` to a boolean.
	 *
	 * The default implementation will call `run`, and then convert the result
	 * to a boolean.
	 */
	kn_boolean (*to_boolean)(void *data);

	/*
	 * Converts the `data` to a string.
	 *
	 * The default implementation will call `run`, and then convert the result
	 * to a string.
	 */
	struct kn_string *(*to_string)(void *data);
};

struct kn_custom {
	void *data;
	const struct kn_custom_vtable *vtable;
};

#endif /* KN_EXT_CUSTOM_TYPES */
#endif /* !KN_CUSTOM_H */
