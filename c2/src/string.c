#include "string.h"
#include "shared.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


#define MAXLENGTH 10
#define CACHESIZE 65536

static struct kn_string_t * STRINGCACHE[MAXLENGTH][CACHESIZE];

const char *kn_string_deref(const struct kn_string_t *string) {
	return string->str;
}

size_t kn_string_length(const struct kn_string_t *string) {
	return string->length;
}


const struct kn_string_t *kn_string_tail(const struct kn_string_t *string, size_t start) {
	return kn_string_emplace(&string->str[start], string->length - start);
	assert(0 <= (ssize_t) start);
	struct kn_string_t *result;

	result = xmalloc(sizeof(struct kn_string_t));

	result->length = string->length - start;
	assert(0 <= (ssize_t) result->length);
	result->refcount = string->refcount;
	result->str = &string->str[start];
	kn_string_clone(string);
	// DEBUG("allocated: %s\n", result->str);

	return result;
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

static struct kn_string_t **get_string(const char *str, size_t length) {
	assert(str != NULL);

	// This is the MurmurHash.
	unsigned long hash = 525201411107845655;

	while (*str != '\0') {
		hash ^= *str++;
		hash *= 0x5bd1e9955bd1e995;
		hash ^= hash >> 47;
	}

	return &STRINGCACHE[length][hash & (CACHESIZE - 1)];
}


const struct kn_string_t *kn_string_emplace(const char *str, size_t length) {
	struct kn_string_t *string, **cacheline;

	assert(0 <= (ssize_t) length);
	assert(str != NULL);

	// if it's too big just dont cache it (as it's unlikely to be referenced again)
	if (length >= MAXLENGTH)
		return create_string(str, length);

	cacheline = get_string(str, length);

	if (*cacheline != NULL && strcmp((string = *cacheline)->str, str) == 0)
		return kn_string_clone(string);
	else
		return *cacheline = create_string(str, length);
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
