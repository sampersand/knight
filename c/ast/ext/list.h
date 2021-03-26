#ifndef KN_EXT_LIST_H
#define KN_EXT_LIST_H
#include "knight/custom.h"
#include "knight/function.h"

struct kn_ext_list_t {
	unsigned length, capacity;
	kn_value_t *data;
};

struct kn_ext_list_t kn_ext_list_alloc(unsigned);
struct kn_ext_list_t kn_ext_list_free(struct kn_ext_list_t kn_ext_list_t);

#endif /* !KN_EXT_LIST_H */