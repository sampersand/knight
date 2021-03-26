#ifndef KN_EXT_LIST_H
#define KN_EXT_LIST_H
#include "knight/custom.h"
#include "knight/function.h"

struct kn_list {
	unsigned length, capacity;
	kn_value *data;
};

struct kn_list kn_list_alloc(unsigned);
struct kn_list kn_list_free(struct kn_list kn_list);

#endif /* !KN_EXT_LIST_H */