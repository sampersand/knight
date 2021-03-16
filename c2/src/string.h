#ifndef KN_STRING_H
#define KN_STRING_H

#include <stddef.h> /* size_t */
#include <limits.h>

/* The string is freed, and can be reclaimed */

#define KN_STRING_EMBEDDED_LENGTH ((sizeof(const char *) + sizeof(size_t)))
/*
 * The string type in Knight.
 *
 * This struct is created via `kn_string_new`, and should be passed to 
 * `kn_string_free` when it is no longer needed.
 */
struct kn_string_t {
	/*
	 * Either the amount of references to this string _or_ what kind it is.
	 * 
	 * For strings that have a positive refcount, they're `malloc`'d, and should
	 * be freed. For all other strings, their `kind` dictates what they are.
	 */
	int refcount;

	union {
		char embedded[KN_STRING_EMBEDDED_LENGTH];

		struct {
			/*
			 * The actual data associated with this string.
			 */
			char *allocated;

			/*
			 * The length of this string; This should e the same as 
			 * `strlen(str)`, and is used for optimization.
			 */
			size_t length;
		};
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



#define KN_STRING_KIND_STATIC INT_MIN

#define KN_STRING_NEW_STATIC() \
	((struct kn_string_t) { .refcount = KN_STRING_KIND_STATIC })

#define KN_STRING_NEW_EMBED(data) \
	((struct kn_string_t) { \
		.refcount = ~(sizeof(data) - 1), \
		.embedded = data \
	})

size_t kn_string_length(const struct kn_string_t *string);
char *kn_string_deref(struct kn_string_t *string);

struct kn_string_t *kn_string_new(char *str, size_t length);

void kn_string_free(struct kn_string_t *string);
struct kn_string_t *kn_string_clone(struct kn_string_t *string);
struct kn_string_t *kn_string_clone_static(struct kn_string_t *string);

#endif
