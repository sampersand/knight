#ifndef KN_STRING_H
#define KN_STRING_H

#include <stddef.h> /* size_t */

/*
 * The string type in Knight.
 *
 * This struct is created via `kn_string_new`, and should be passed to 
 * `kn_string_free` when it is no longer needed.
 *
 * A negative `refcount` implies that this struct doesn't own its data, and
 * should not `free` it. when finished.
 */
struct kn_string_t {
	/*
	 * The length of this string; this should be always the same
	 */
	const size_t length;
	int refcount;
	const char *str;
};

static struct kn_string_t KN_STRING_EMPTY = { 0, -1, "" };


void kn_string_startup(void);
void kn_string_shutdown(void);
struct kn_string_t *kn_string_new(const char *start, size_t length);

void kn_string_free(struct kn_string_t *string);
struct kn_string_t *kn_string_clone(struct kn_string_t *string);

#ifndef NDEBUG
_Bool kn_string_is_interned(const struct kn_string_t* string);
#endif /* NDEBUG */

#endif
