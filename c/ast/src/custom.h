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
	 * Duplicates `custom`.
	 *
	 * The return value must be passed to `free` separately from `custom`.
	 *
	 * This function is required and does not have a default value.
	 */
	struct kn_custom *(*clone)(struct kn_custom *custom);

	/*
	 * Releases the resources associated with `data`.
	 *
	 * Note that this function is responsible for calling `free` on the `custom`
	 * pointer too when releasing resources.
	 */
	void (*free)(struct kn_custom *custom);

	/*
	 * Dumps debugging info for `data`.
	 *
	 * The default implementation simply prints out
	 * `Custom(<data ptr>, <vtable ptr>)`.
	 */
	void (*dump)(struct kn_custom *custom);

	/*
	 * Executes the given `custom`, returning the value associated with it.
	 *
	 * This is the only function that accepts a `kn_custom`, as you may want
	 * to somehow return the custom itself.
	 *
	 * The default implementation simply duplicates `custom`.
	 */
	kn_value (*run)(struct kn_custom *custom);

	/*
	 * Converts the `data` to a number.
	 *
	 * The default implementation will `run` the associated `custom`, and then
	 * convert its result to a string.
	 */
	kn_number (*to_number)(struct kn_custom *custom);

	/*
	 * Converts the `data` to a boolean.
	 */
	kn_boolean (*to_boolean)(struct kn_custom *custom);

	/*
	 * Converts the `data` to a string.
	 */
	struct kn_string *(*to_string)(struct kn_custom *custom);
};

struct kn_custom {
	void *data;
	const struct kn_custom_vtable *vtable;
};

#endif /* KN_EXT_CUSTOM_TYPES */
#endif /* !KN_CUSTOM_H */
