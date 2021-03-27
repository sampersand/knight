#ifndef KN_EXT_LIST_H
#define KN_EXT_LIST_H

#include "../src/value.h"
#include "../src/custom.h"

struct kn_list;

struct kn_list *kn_list_alloc(unsigned);
kn_value *kn_list_deref(unsigned);

struct kn_list *kn_list_clone(struct kn_list *);
void kn_list_free(struct kn_list *);

kn_value kn_list_car(struct kn_list *);
struct kn_list *kn_list_cdr(struct kn_list *);

// struct kn_custom_vtable kn_list_vtable = {
// 	clone = (void *(*)(void *)) kn_list_clone,
// 	free = 
// 	struct kn_custom *(*clone)(struct kn_custom *);
// 	void (*free)(struct kn_custom *);
// 	void (*dump)(struct kn_custom *);
// 	kn_value (*run)(struct kn_custom *);
// 	kn_number (*to_number)(struct kn_custom *);
// 	kn_boolean (*to_boolean)(struct kn_custom *);
// 	struct kn_string *(*to_string)(struct kn_custom *);
// };

#endif /* !KN_EXT_LIST_H */