#include "string.h"
#include "shared.h"
#include <stdio.h>
#include "assert.h"

struct kn_string_t *kn_string_new(char *string) {
	assert(string != NULL);

	struct kn_string_t *ret = xmalloc(sizeof(struct kn_string_t));
	ret->refcount = 1;
	ret->string = string;

	return ret;
}

void kn_string_free(struct kn_string_t *string) {
	assert(string != NULL);

	if (string->refcount && !--string->refcount) {
		free(string->string);
		free(string);
	}
}

struct kn_string_t *kn_string_clone(struct kn_string_t *string) {
	assert(string != NULL);

	if (string->refcount)
		++string->refcount;

	return string;
}
