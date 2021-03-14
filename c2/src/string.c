#include "string.h"
#include "shared.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


#define MAXLENGTH 64
#define CACHESIZE 65536

static struct kn_string_t *STRINGCACHE[MAXLENGTH][CACHESIZE];

inline const char *kn_string_deref(const struct kn_string_t *string) {
	return string->str;
}

inline size_t kn_string_length(const struct kn_string_t *string) {
	return string->length;
}


const struct kn_string_t *kn_string_tail(const struct kn_string_t *string, size_t start) {
	return kn_string_emplace(&string->str[start], string->length - start);
	// assert(0 <= (ssize_t) start);
	// struct kn_string_t *result;

	// result = xmalloc(sizeof(struct kn_string_t));

	// result->length = string->length - start;
	// assert(0 <= (ssize_t) result->length);
	// result->refcount = string->refcount;
	// result->str = &string->str[start];
	// kn_string_clone(string);
	// // DEBUG("allocated: %s\n", result->str);

	// return result;
}

static struct kn_string_t *create_string(const char *str, size_t length) {
	struct kn_string_t *string;

	string = xmalloc(sizeof(struct kn_string_t));

	string->length = length;
	string->refcount = xmalloc(sizeof(unsigned));
	string->str = str;

	// DEBUG("allocated: %s\n", string->str);

	return string;
}

const struct kn_string_t *kn_string_emplace(const char *str, size_t length) {
	struct kn_string_t *string, **cacheline;

	// sanity check for inputs.
	assert(0 <= (ssize_t) length);
	assert(str != NULL);
	assert(strlen(str) == length);

	// if it's too big just dont cache it
	// (as it's unlikely to be referenced again)
	if (length >= MAXLENGTH)
		return create_string(str, length);

	
	cacheline = &STRINGCACHE[length][kn_hash(str) & (CACHESIZE - 1)];

	if (*cacheline == NULL || strcmp((string = *cacheline)->str, str))
		return *cacheline = create_string(str, length);

	assert(string->refcount != NULL);
	++*string->refcount;

	return string;
}

const struct kn_string_t *kn_string_new(const char *str) {
	assert(str != NULL);

	return kn_string_emplace(str, strlen(str));
}

void kn_string_free(const struct kn_string_t *string) {
	assert(string != NULL);

	struct kn_string_t *unconst = (struct kn_string_t *) string;

	if (string->refcount != NULL && !--*unconst->refcount) {
		// free((void *) unconst->str);
		// free(unconst);
	}
}

const struct kn_string_t *kn_string_clone(const struct kn_string_t *string) {
	assert(string != NULL);

	if (string->refcount != NULL)
		++*((struct kn_string_t *) string)->refcount;

	return string;
}
