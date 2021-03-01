#include <assert.h> /* assert */
#include <stdlib.h> /* free */

#include "shared.h" /* xmalloc */
#include "string.h" /* prototypes, kn_string_t */

// not technically "interning" a string, but rather is used for string literals.
struct kn_string_t kn_string_intern(const char *str) {
	assert(str != NULL);

	return (struct kn_string_t) {
		.str = str,
		.rc = NULL
	};
}

struct kn_string_t kn_string_new(const char *str) {
	assert(str != NULL);

	unsigned *rc = xmalloc(sizeof(unsigned));
	*rc = 1;

	return (struct kn_string_t) {
		.str = str,
		.rc = rc
	};
}

struct kn_string_t kn_string_clone(const struct kn_string_t *string) {
	assert(string != NULL);

	// We accept a const pointer because `kn_string_clone` should obviously
	// be callable on a const pointer. However, we have interior mutability,
	// so we cast away the constness inside.
	if (string->rc != NULL) {
		++*((struct kn_string_t *) string)->rc;
	}

	return *string;
}

void kn_string_free(struct kn_string_t *string) {
	assert(string != NULL);

	// We own the string now, so we're free to remove its constness.
	if (string->rc != NULL && --(*(string->rc)) == 0) {
		free((char *) string->str);
		free(string->rc);
	}
}
