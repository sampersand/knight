#include "string.h"
#include "shared.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

struct kn_string_t *kn_string_alloc(size_t length) {
	// `+ 1` because of trailing NUL.
	struct kn_string_t *string = xmalloc(
		sizeof(struct kn_string_t) + sizeof(char [length + 1]));

	string->length = length;
	string->refcount = 1;

	return string;
}

const struct kn_string_t *kn_string_emplace(const char *str, size_t length) {
	assert(str != NULL);

	struct kn_string_t *string = kn_string_alloc(length);
	memcpy(string->str, str, length);
	string->str[length] = '\0';

	return string;
}

const struct kn_string_t *kn_string_new(const char *str) {
	assert(str != NULL);

	return kn_string_emplace(str, strlen(str));
}

void kn_string_free(const struct kn_string_t *string) {
	assert(string != NULL);

	struct kn_string_t *unconst = (struct kn_string_t *) string;

	if (unconst->refcount && !--unconst->refcount)
		// free(unconst);
		;
}

const struct kn_string_t *kn_string_clone(const struct kn_string_t *string) {
	assert(string != NULL);

	if (string->refcount)
		++((struct kn_string_t *) string)->refcount;

	return string;
}
