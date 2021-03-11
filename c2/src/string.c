#include "string.h"
#include "shared.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

const struct kn_string_t *kn_string_tail(
	const struct kn_string_t *string,
	size_t start
) {
	struct kn_string_t *result;

	result = xmalloc(sizeof(struct kn_string_t));

	result->length = string->length - start;
	result->refcount = string->refcount;
	result->str = string->str + start;
	kn_string_clone(string);

	return result;
}

const struct kn_string_t *kn_string_emplace(const char *str, size_t length) {
	struct kn_string_t *string;

	assert(str != NULL);

	string = xmalloc(sizeof(struct kn_string_t));

	string->length = length;
	string->refcount = xmalloc(sizeof(unsigned));
	string->str = str;

	return string;
}

const struct kn_string_t *kn_string_new(const char *str) {
	assert(str != NULL);

	return kn_string_emplace(str, strlen(str));
}

void kn_string_free(const struct kn_string_t *string) {
	struct kn_string_t *unconst;

	assert(string != NULL);

	unconst = (struct kn_string_t *) string;

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
