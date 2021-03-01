#ifndef KN_STRING_H
#define KN_STRING_H

#include <stdlib.h>

struct kn_string_t {
	intptr_t length;
	unsigned refcount;
	char str[];
};

static struct kn_string_t KN_STRING_TRUE = {
	4, 0, {'t', 'r', 'u', 'e', '\0'}
};

static struct kn_string_t KN_STRING_FALSE = {
	5, 0, {'f', 'a', 'l', 's', 'e', '\0'}
};

static struct kn_string_t KN_STRING_NULL = {
	4, 0, {'n', 'u', 'l', 'l', '\0'}
};

static struct kn_string_t KN_STRING_EMPTY = {
	0, 0, {'\0'}
};

static struct kn_string_t KN_STRING_ZERO = {
	1, 0, {'0', '\0'}
};
static struct kn_string_t KN_STRING_ONE = {
	1, 0, {'1', '\0'}
};

const struct kn_string_t *kn_string_new(const char *);
struct kn_string_t *kn_string_alloc(size_t);
const struct kn_string_t *kn_string_emplace(const char *, size_t);

void kn_string_free(const struct kn_string_t *);
const struct kn_string_t *kn_string_clone(const struct kn_string_t *);

#endif
