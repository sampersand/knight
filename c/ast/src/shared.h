#ifndef KN_SHARED_H
#define KN_SHARED_H

#include <stddef.h> /* size_t */
#include <stdlib.h> /* abort */

#ifdef KN_USE_EXTENSIONS
# define KN_ATTRIBUTE(x) __attribute__(x)
# define KN_EXPECT(x, y) (__builtin_expect(x, y))
# ifdef NDEBUG
#  define KN_UNREACHABLE() (__builtin_unreachable())
# else
#  define KN_UNREACHABLE() \
	die("Issue at  %s:%s:%d", __FILE__, __FUNCTION__, __LINE__)
# endif /* NDEBUG */
#else
# define KN_EXPECT(x, y) (x)
# define KN_ATTRIBUTE(x)
# ifdef NDEBUG
#  define KN_UNREACHABLE() (abort())
# else
#  define KN_UNREACHABLE() \
	die("Issue at  %s:%s:%d", __FILE__, __FUNCTION__, __LINE__)
# endif /* NDEBUG */
#endif /* KN_USE_EXTENSIONS */
	
#define KN_LIKELY(x) (__builtin_expect(!!(x), 1))
#define KN_UNLIKELY(x) (__builtin_expect(!!(x), 0))


/*
 * A function that's used to halt the execution of the program, writing the
 * given message to stderr before exiting with code 1.
 *
 * Since this aborts the program, it's marked both `noreturn` and `cold`.
 */
void die(const char *msg, ...) KN_ATTRIBUTE((noreturn,cold));

/*
 * Returns a hash for the given string.
 */
unsigned long kn_hash(const char *str);

/*
 * Allocates `size` bytes of memory and returns a pointer to them.
 *
 * This is identical to the stdlib's `malloc`, except the program is aborted
 * instead of returning `NULL`.
 *
 * The `size`, when converted to an `ssize_t`, must be nonnegative.
 */
void *xmalloc(size_t size) KN_ATTRIBUTE((malloc));

/*
 * Resizes the pointer to a segment of at least `size` bytes of memory and
 * returns the new segment's pointer.
 *
 * This is identical to the stdlib's `realloc`, except the program is aborted
 * instead of returning `NULL`.
 *
 * The `size`, when converted to an `ssize_t`, must be nonnegative.
 */
void *xrealloc(void *ptr, size_t size);

#ifdef KN_COMPUTED_GOTOS
# define KN_CGOTO_SWITCH(value, tabel) goto *tabel[(size_t) value];
# define KN_CGOTO_DEFAULT(lbl) lbl
# define KN_CGOTO_CASE(lbl, ...) lbl
# define KN_CGOTO_LABEL(lbl) lbl:
# define KN_CGOTO_CASES(...)
#else
# define KN_CGOTO_SWITCH(value, tabel) switch(value)
# define KN_CGOTO_DEFAULT(lbl) default
# define KN_CGOTO_CASE(lbl, ...) KN_CGOTO_CASES_(__VA_ARGS__)
# define KN_CGOTO_LABEL(lbl)
# define KN_CGOTO_CASES(...) KN_CGOTO_CASES_(__VA_ARGS__):
# define KN_CGOTO_CASES_(...) KN_CG_(__VA_ARGS__, \
	16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0)(__VA_ARGS__)
# define KN_CG_(n0,n1,n2,n3,n4,n5,n6,n7,n8,n9,n10,n11,n12,n13,n14,n15,n,...) \
	KN_CG_##n
# define KN_CG_15(cse, ...) case cse: KN_CG_14(__VA_ARGS__)
# define KN_CG_14(cse, ...) case cse: KN_CG_13(__VA_ARGS__)
# define KN_CG_13(cse, ...) case cse: KN_CG_12(__VA_ARGS__)
# define KN_CG_12(cse, ...) case cse: KN_CG_11(__VA_ARGS__)
# define KN_CG_11(cse, ...) case cse: KN_CG_10(__VA_ARGS__)
# define KN_CG_10(cse, ...) case cse: KN_CG_9(__VA_ARGS__)
# define KN_CG_9(cse, ...) case cse: KN_CG_8(__VA_ARGS__)
# define KN_CG_8(cse, ...) case cse: KN_CG_7(__VA_ARGS__)
# define KN_CG_7(cse, ...) case cse: KN_CG_6(__VA_ARGS__)
# define KN_CG_6(cse, ...) case cse: KN_CG_5(__VA_ARGS__)
# define KN_CG_5(cse, ...) case cse: KN_CG_4(__VA_ARGS__)
# define KN_CG_4(cse, ...) case cse: KN_CG_3(__VA_ARGS__)
# define KN_CG_3(cse, ...) case cse: KN_CG_2(__VA_ARGS__)
# define KN_CG_2(cse, ...) case cse: KN_CG_1(__VA_ARGS__)
# define KN_CG_1(cse) case cse
#endif /* KN_COMPUTED_GOTOS */

#endif /* !KN_SHARED_H */
