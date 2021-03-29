#ifndef KN_EXT_LIST_H
#define KN_EXT_LIST_H

#include "../src/value.h"
#include "../src/custom.h"
#include <stdbool.h>

extern const struct kn_custom_vtable kn_list_vtable;

struct kn_list {
	kn_value data;
	int refcount;
	struct kn_list *next;
};

struct kn_list kn_list_empty = {
	.data = KN_UNDEFINED,
	.refcount = -1,
	.next = &kn_list_empty
};

struct kn_list *kn_list_new(kn_value);
struct kn_list *kn_list_clone(struct kn_list *);
void kn_list_free(struct kn_list *);
void kn_list_dump(const struct kn_list *);


bool kn_list_is_empty(const struct kn_list *);
kn_value kn_list_car(struct kn_list *);
struct kn_list *kn_list_cdr(struct kn_list *);
struct kn_list *kn_list_cons(struct kn_list *, struct kn_list *);
struct kn_list *kn_list_run(struct kn_list *);

kn_value kn_fn_extension_parse(const char **stream);

#endif /* !KN_EXT_LIST_H */

