#ifndef KN_SHARED_H
#define KN_SHARED_H

#include <stddef.h> /* size_t */

/*
 * A function that's used to halt the execution of the program, writing the
 * given message to stderr before exiting with code 1.
 *
 * Since this aborts the program, it's marked both `noreturn` and `cold`.
 */
void die(const char *msg, ...) __attribute__((noreturn,cold));

#ifdef KN_RECKLESS
# define assert_reckless(_) \
	do { } while(0)
#else
# define assert_reckless(value) \
	do { if (!(value)) die("invalid value: %s", #value); } while(0)
#endif /* KN_RECKLESS */

/*
 * Returns a hash for the given string.
 */
unsigned long kn_hash(const char *str);

/*
 * Allocates `size_t` bytes of memory and returns a pointer to it.
 *
 * This is identical to the stdlib's `malloc`, except the program is aborted
 * instead of returning `NULL`.
 *
 * The `size`, when converted to an `ssize_t`, must be nonnegative.
 */
void *xmalloc(size_t size) __attribute__((malloc));

/*
 * Resizes the pointer to a segment of at least `size_t` bytes of memory and,
 * returning the new segment's pointer.
 *
 * This is identical to the stdlib's `realloc`, except the program is aborted
 * instead of returning `NULL`.
 *
 * The `size`, when converted to an `ssize_t`, must be nonnegative.
 */
void *xrealloc(void *ptr, size_t size);

#endif /* KN_SHARED_H */
