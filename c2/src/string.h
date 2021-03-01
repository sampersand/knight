#ifndef KN_STRING_H
#define KN_STRING_H

#include <stdlib.h>

struct kn_string_t {
	char *string;
	unsigned refcount;
};

static struct kn_string_t KN_STRING_TRUE = { "true", 0 };
static struct kn_string_t KN_STRING_FALSE = { "false", 0 };
static struct kn_string_t KN_STRING_NULL = { "null", 0 };
static struct kn_string_t KN_STRING_EMPTY = { "", 0 };
static struct kn_string_t KN_STRING_ZERO = { "0", 0 };
static struct kn_string_t KN_STRING_ONE = { "1", 0 };

struct kn_string_t *kn_string_new(char *);
void kn_string_free(struct kn_string_t *);
struct kn_string_t *kn_string_clone(struct kn_string_t *);

#endif
