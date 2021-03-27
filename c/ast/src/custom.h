#ifndef KN_CUSTOM_H
#define KN_CUSTOM_H
#ifdef KN_EXT_CUSTOM_TYPES

#include "value.h" /* kn_value, kn_number, kn_boolean, kn_string */

/*
 * The virtual table for custom types.
 *
 * There should generally only be one of these per type.
 */
struct kn_custom_vtable {
	/*
	 * Releases the resources associated with `data`.
	 */
	void (*free)(void *data);

	/*
	 * Dumps debugging info for `data`.
	 */
	void (*dump)(void *data);

	/*
	 * Executes the given `custom`, 
	 */
	kn_value (*run)(struct kn_custom_t *);
	kn_number (*to_number)(void *data);
	kn_boolean (*to_boolean)(void *data);
	struct kn_string *(*to_string)(struct kn_custom *custom);
};

struct kn_custom {
	void *data;
	unsigned refcount;
	const struct kn_custom_vtable *vtable;
};

#endif /* KN_EXT_CUSTOM_TYPES */
#endif /* !KN_CUSTOM_H */
