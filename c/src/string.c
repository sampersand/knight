#include "string.h"
#include "shared.h"

#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

struct kn_string_t kn_string_intern(const char *str) {
	assert(str != NULL);

	return (struct kn_string_t) { .str = str, .rc = NULL };
}

struct kn_string_t kn_string_new(const char *str) {
	assert(str != NULL);

	unsigned *rc = xmalloc(sizeof(unsigned));
	*rc = 1;

	return (struct kn_string_t) { .str = str, .rc = rc };
}

struct kn_string_t kn_string_clone(const struct kn_string_t *string) {
	assert(string != NULL);

	if (string->rc != NULL) {
		((struct kn_string_t *) string)->rc++;
	}

	return *(struct kn_string_t *) string;
}

void kn_string_free(struct kn_string_t *string) {
	assert(string != NULL);

	if (string->rc != NULL && --string->rc == 0) {
		free((char *) string->str);
	}
}
