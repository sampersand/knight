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
	int refcount;
	const char *str;
};

static struct kn_string_t KN_STRING_EMPTY = (struct kn_string_t) { 0, 0, "" };


// these are deprecated.
size_t kn_string_length(const struct kn_string_t *);
const char *kn_string_deref(const struct kn_string_t *);

struct kn_string_t *kn_string_tail(struct kn_string_t *, size_t);
struct kn_string_t *kn_string_new(const char *);
struct kn_string_t *kn_string_emplace(const char *, size_t);

void kn_string_free(struct kn_string_t *);
struct kn_string_t *kn_string_clone(struct kn_string_t *);

#endif
