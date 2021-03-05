#ifndef STRING_H
#define STRING_H

typedef struct {
	unsigned length;
	unsigned *refcount;
	char *str;
} kn_string_t;

static kn_string_t KN_STRING_TRUE = { 4, 0, "true" };
static kn_string_t KN_STRING_FALSE = { 5, 0, "false" };
static kn_string_t KN_STRING_NULL = { 4, 0, "null" };
static kn_string_t KN_STRING_EMPTY = { 0, 0, "" };
static kn_string_t KN_STRING_ZERO = { 1, 0, "0" };
static kn_string_t KN_STRING_ONE = { 1, 0, "1" };

kn_string_t *kn_string_tail(kn_string_t *, unsigned);
kn_string_t *kn_string_new(char *);
kn_string_t *kn_string_emplace(char *, unsigned);

void kn_string_free(kn_string_t *);
kn_string_t *kn_string_clone(kn_string_t *);

#endif
