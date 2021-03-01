#include "string.h"
#include "shared.h"
#include <stdio.h>
#include "assert.h"

const struct kn_string_t *kn_string_new(char *string) {
	assert(string != NULL);

	struct kn_string_t *ret = xmalloc(sizeof(struct kn_string_t));
	ret->refcount = 1;
	ret->str = string;

	return ret;
}

void kn_string_free(const struct kn_string_t *string) {
	assert(string != NULL);

	struct kn_string_t *unconst = (struct kn_string_t *) string;

	if (unconst->refcount && !--unconst->refcount) {
		// free(unconst->str);
		// free(unconst);
	}
}

const struct kn_string_t *kn_string_clone(const struct kn_string_t *string) {
	assert(string != NULL);
	struct kn_string_t *unconst = (struct kn_string_t *) string;


	if (string->refcount)
		++unconst->refcount;

	return string;
}
