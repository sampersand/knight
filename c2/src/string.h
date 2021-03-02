#ifndef KN_STRING_H
#define KN_STRING_H

#include <stdlib.h>

struct kn_string_t {
	intptr_t length;
	unsigned *refcount;
	const char *str;
};

static struct kn_string_t KN_STRING_TRUE = { 4, 0, "true" };
static struct kn_string_t KN_STRING_FALSE = { 5, 0, "false" };
static struct kn_string_t KN_STRING_NULL = { 4, 0, "null" };
static struct kn_string_t KN_STRING_EMPTY = { 0, 0, "" };
static struct kn_string_t KN_STRING_ZERO = { 1, 0, "0" };
static struct kn_string_t KN_STRING_ONE = { 1, 0, "1" };

const struct kn_string_t *kn_string_tail(const struct kn_string_t *, size_t);
const struct kn_string_t *kn_string_new(const char *);
const struct kn_string_t *kn_string_emplace(const char *, size_t);

void kn_string_free(const struct kn_string_t *);
const struct kn_string_t *kn_string_clone(const struct kn_string_t *);

#endif
