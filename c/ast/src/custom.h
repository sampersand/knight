#ifndef KN_CUSTOM_H
#define KN_CUSTOM_H
#ifdef KN_EXT_CUSTOM_TYPES

#include "value.h" /* kn_value, kn_number, kn_boolean, kn_string */

struct kn_custom_vtable {
	struct kn_custom *(*clone)(struct kn_custom *);
	void (*free)(struct kn_custom *);
	void (*dump)(struct kn_custom *);
	kn_value (*run)(struct kn_custom *);
	kn_number (*to_number)(struct kn_custom *);
	kn_boolean (*to_boolean)(struct kn_custom *);
	struct kn_string *(*to_string)(struct kn_custom *);
};

struct kn_custom {
	void *data;
	const struct kn_custom_vtable *vtable;
};

#endif /* KN_EXT_CUSTOM_TYPES */
#endif /* !KN_CUSTOM_H */
