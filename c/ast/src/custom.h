#ifndef KN_CUSTOM_H
#define KN_CUSTOM_H
#ifdef KN_EXT_CUSTOM_TYPES
#include "value.h" /* kn_value_t, kn_number_t, kn_boolean_t, kn_string_t */

struct kn_custom_vtable_t {
	struct kn_custom_t *(*clone)(struct kn_custom_t *);
	void (*free)(struct kn_custom_t *);
	void (*dump)(struct kn_custom_t *);
	kn_value_t (*run)(struct kn_custom_t *);
	kn_number_t (*to_number)(struct kn_custom_t *);
	kn_boolean_t (*to_boolean)(struct kn_custom_t *);
	struct kn_string_t *(*to_string)(struct kn_custom_t *);
};

struct kn_custom_t {
	void *data;
	const struct kn_custom_vtable_t *vtable;
};

#endif /* KN_EXT_CUSTOM_TYPES */
#endif /* !KN_CUSTOM_H */
