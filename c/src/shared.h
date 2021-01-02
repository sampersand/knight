#ifndef KN_SHARED_H
#define KN_SHARED_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>


#ifndef NDEBUG
#define assert_msg(cond, msg, ...) \
	do { \
		if (!(cond)) { \
			bug("assertion failed: " msg, ##__VA_ARGS__); \
		} \
	} while (0)
#else
#define DEBUG_ASSERT(cond, msg, ...) \
	do { \
		/* nothing */ \
	} while(0)
#endif /* DEBUG */


#ifdef DEBUG
#define DEBUG_ASSERT(cond, msg, ...) \
	do { \
		if (!(cond)) { \
			bug("assertion failed: " msg, ##__VA_ARGS__); \
		} \
	} while (0)
#else
#define DEBUG_ASSERT(cond, msg, ...) \
	do { \
		/* nothing */ \
	} while(0)
#endif /* DEBUG */

/*
 * A function that's used to halt the execution of the program, writing the
 * given message to stderr before exiting with code 1.
 *
 * Since this aborts the program, it's marked both `noreturn` and `cold`.
 */
void die(const char *, ...) __attribute__((noreturn,cold));

/* 
 * A macro that's used to indicate an internal bug occurred. This macro
 * _should_ never be executed, as doing so indicates a bug in the program.
 */
#define bug(msg, ...) \
	die("%s:%s:%d: bug encountered: " msg "\n", \
		__FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)

/*
 * Allocates `size_t` bytes of memory and returns a pointer to it.
 *
 * This is identical to the stdlib's `malloc`, except the program is aborted
 * instead of returning `NULL`.
 */
void *xmalloc(size_t);

/*
 * Resizes the pointer to a segment of at least `size_t` bytes of memory and,
 * returning the new segment's pointer.
 *
 * This is identical to the stdlib's `realloc`, except the program is aborted
 * instead of returning `NULL`.
 */
void *xrealloc(void *, size_t);

/*
 * Frees the given pointer.
 *
 * This is identical to the stdlib's `free`, except the program is aborted if
 * `NULL` is passed.
 *
 * Unlike the other two `x` functions, `NULL` isn't expected: If one is
 * encountered, it's considered a bug. 
 */
void xfree(void *);

/*
 * A Macro used to verify that a pointer's not null. If it is, we abort
 * with a message.
 *
 * (Note: this is a macro so it can handle all different types of `ptr`s.)
 */
#define VERIFY_NOT_NULL(ptr, msg, ...) \
	do { \
		if ((ptr) == NULL) { \
			bug("null pointer encountered: " msg, ##__VA_ARGS__); \
		} \
	} while (0)

#endif
