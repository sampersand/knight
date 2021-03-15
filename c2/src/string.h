#ifndef KN_STRING_H
#define KN_STRING_H

#include <stdlib.h>

struct kn_string_t {
	size_t length;
	int refcount;
	const char *str;
};

static struct kn_string_t KN_STRING_EMPTY = (struct kn_string_t) { 0, -1, "" };


void kn_string_startup(void);
void kn_string_shutdown(void);
struct kn_string_t *kn_string_tail(struct kn_string_t *string, size_t start);
struct kn_string_t *kn_string_new(const char *start, size_t length);

void kn_string_free(struct kn_string_t *string);
struct kn_string_t *kn_string_clone(struct kn_string_t *string);

#ifndef NDEBUG
_Bool kn_string_is_interned(const struct kn_string_t* string);
#endif /* NDEBUG */

#endif
