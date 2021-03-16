#ifndef KN_STRING_H
#define KN_STRING_H

#include <stddef.h> /* size_t */

/*
 * This enum is used to represent the kind of `kn_string_t`.
 *
 * These are special cases of the `refcount` of a string.
 */
enum kn_string_kind_t {
	/* The string is freed, and can be reclaimed */
	KN_STRING_KIND_FREE = 0,

	/* The string is an internal string, such as `KN_STRING_EMPTY */
	KN_STRING_KIND_INTERN = -1,

	/* The string is not allocated, but it _should_ be duplicated if cloned. */
	KN_STRING_KIND_STATIC = -2
};

/*
 * The string type in Knight.
 *
 * This struct is created via `kn_string_new`, and should be passed to 
 * `kn_string_free` when it is no longer needed.
 */
struct kn_string_t {
	/*
	 * The actual data associated with this string.
	 */
	const char *str;

	/*
	 * The length of this string; This should e the same as `strlen(str)`, and
	 * is used for optimization.
	 */
	size_t length;

	/*
	 * The amount of references to this string that exist.
	 *
	 * A `refcount` of `-1` implies that this struct doesn't own its data, and
	 * should not `free` it. when finished.
	 *
	 * A `refcount` of _exaclty_ `-2` indicates that the value shouldn't be
	 *
	 */
	union {
		int refcount;
		enum kn_string_kind_t kind;
	};
};

/*
 * The empty string.
 */
extern struct kn_string_t KN_STRING_EMPTY;

/*
 * Initializes the string allocations.
 *
 * This should be called before `kn_string_new` is run.
 */
void kn_string_startup(void);
void kn_string_shutdown(void);
struct kn_string_t *kn_string_new(const char *start, size_t length);

void kn_string_free(struct kn_string_t *string);
struct kn_string_t *kn_string_clone(struct kn_string_t *string);
struct kn_string_t *kn_string_clone_static(struct kn_string_t *string);

#endif
