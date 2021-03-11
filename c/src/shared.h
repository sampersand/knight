#ifndef KN_SHARED_H
#define KN_SHARED_H

#include <stdlib.h> /* size_t */

/*
 * A function that's used to halt the execution of the program, writing the
 * given message to stderr before exiting with code 1.
 *
 * Since this aborts the program, it's marked both `noreturn` and `cold`.
 */
void die(const char *msg, ...) __attribute__((noreturn,cold));

/* 
 * A macro that's used to indicate an internal bug occurred. This macro
 * _should_ never be executed, as doing so indicates a bug in the program.
 */
#define bug(msg, ...) \
	die("%s:%s:%d: bug encountered: " msg "\n", \
		__FILE__, __func__, __LINE__, __VA_ARGS__)

/*
 * Allocates `size_t` bytes of memory and returns a pointer to it.
 *
 * This is identical to the stdlib's `malloc`, except the program is aborted
 * instead of returning `NULL`.
 */
void *xmalloc(size_t size);

/*
 * Resizes the pointer to a segment of at least `size_t` bytes of memory and,
 * returning the new segment's pointer.
 *
 * This is identical to the stdlib's `realloc`, except the program is aborted
 * instead of returning `NULL`.
 */
void *xrealloc(void *ptr, size_t size);

#endif /* KN_SHARED_H */
