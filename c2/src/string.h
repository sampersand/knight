#ifndef KN_STRING_H
#define KN_STRING_H

#include <stdlib.h>

struct kn_string_t {
	char *str;
	unsigned refcount;
	// TODO: put a length here.
};

static struct kn_string_t KN_STRING_TRUE = { "true", 0 };
static struct kn_string_t KN_STRING_FALSE = { "false", 0 };
static struct kn_string_t KN_STRING_NULL = { "null", 0 };
static struct kn_string_t KN_STRING_EMPTY = { "", 0 };
static struct kn_string_t KN_STRING_ZERO = { "0", 0 };
static struct kn_string_t KN_STRING_ONE = { "1", 0 };

const struct kn_string_t *kn_string_new(char *);
void kn_string_free(const struct kn_string_t *);
const struct kn_string_t *kn_string_clone(const struct kn_string_t *);

#endif
