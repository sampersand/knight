#include <stdlib.h>
#include "list.h"
#include "../src/custom.h"
#include "../src/shared.h"
#include "../src/function.h"

struct kn_list {
	unsigned capacity, length, refcount;
	kn_value *data;
};

struct kn_list kn_list_empty;

struct kn_list *kn_list_new(kn_value value) {
	struct kn_list *list = xmalloc(sizeof(struct kn_list));

	list->data = value;
	list->next = NULL;
	list->refcount = 1;

	return list;
}

struct kn_list *kn_list_clone(struct kn_list *list) {
	list->refcount++;

	return list;
}

void kn_list_free(struct kn_list *list) {
	if (--list->refcount)
		return;

	kn_value_free()
}

kn_value kn_list_car(struct kn_list *);
struct kn_list *kn_list_cdr(struct kn_list *);

#endif /* !KN_EXT_LIST_H */