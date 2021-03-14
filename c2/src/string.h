#ifndef KN_STRING_H
#define KN_STRING_H

#include <stdlib.h>

// struct kn_string_t;

// static struct kn_string_t *KN_STRING_TRUE;
// static struct kn_string_t *KN_STRING_FALSE;
// static struct kn_string_t *KN_STRING_NULL;
// static struct kn_string_t *KN_STRING_EMPTY;
// static struct kn_string_t *KN_STRING_ZERO;
// static struct kn_string_t *KN_STRING_ONE;


struct kn_string_t {
	size_t length;
	unsigned *refcount;
	const char *str;
};

static struct kn_string_t KN_STRING_TRUE = (struct kn_string_t) { 4, NULL, "true" };
static struct kn_string_t KN_STRING_FALSE = (struct kn_string_t) { 5, NULL, "false" };
static struct kn_string_t KN_STRING_NULL = (struct kn_string_t) { 4, NULL, "null" };
static struct kn_string_t *KN_STRING_EMPTY = &(struct kn_string_t) { 0, NULL, "" };
static struct kn_string_t KN_STRING_ZERO = (struct kn_string_t) { 1, NULL, "0" };
static struct kn_string_t KN_STRING_ONE = (struct kn_string_t) { 1, NULL, "1" };


size_t kn_string_length(const struct kn_string_t *);
const char *kn_string_deref(const struct kn_string_t *);

const struct kn_string_t *kn_string_tail(const struct kn_string_t *, size_t);
const struct kn_string_t *kn_string_new(const char *);
const struct kn_string_t *kn_string_emplace(const char *, size_t);

void kn_string_free(const struct kn_string_t *);
const struct kn_string_t *kn_string_clone(const struct kn_string_t *);

#endif
