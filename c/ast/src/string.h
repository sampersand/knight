#ifndef KN_STRING_H
#define KN_STRING_H

#include <stddef.h> /* size_t */

/*
 * These flags are used to record information about how the memory of a
 * `kn_string` should be managed.
 */
enum kn_string_flags {
	/*
	 * Indicates that the struct itself was allocated.
	 *
	 * If this is set, when a string is `kn_string_free`d, the struct pointer
	 * itself will also be freed.
	 */
	KN_STRING_FL_STRUCT_ALLOC = 1,

	/*
	 * Indicates that a string's data is stored in the `embed`ded field of
	 * the string, rather than the `alloc`ated  part.
	 */
	KN_STRING_FL_EMBED = 2,

	/*
	 * Indicates that the string is a `static` string---that is, it's not
	 * allocated, but should it should be fully duplicated when the function
	 * `kn_string_clone_static` is called.
	 */
	KN_STRING_FL_STATIC = 4,
};

/*
 * How many bytes of padding to use; the larger the number, the more strings are
 * embedded, but the more memory used.
 */
#ifndef KN_STRING_PADDING_LENGTH
# define KN_STRING_PADDING_LENGTH 8
#endif /* !KN_STRING_PADDING_LENGTH */

/*
 * The length of the embedded segment of the string.
 */
#define KN_STRING_EMBEDDED_LENGTH \
	(sizeof(size_t) \
		+ sizeof(char *) \
		+ sizeof(char [KN_STRING_PADDING_LENGTH]) \
		- 1)

/*
 * The string type in Knight.
 *
 * This struct is generally allocated by a `kn_string_alloc`, which can then be
 * populated via the `kn_string_deref` function.
 *
 * As an optimization, strings can either be `embed`ded (where the `chars` are
 * actually a part of the struct), or `alloc`ated (where the data is stored
 * elsewhere, and a pointer to it is used.)
 *
 * Regardless of the type of string, it should be passed to `kn_string_free` to
 * properly dispose of its resources when you're finished with it.
 */
struct kn_string {
	/* The flags that dictate how to manage this struct's memory. */
	enum kn_string_flags flags;

	/*
	 * The amount of references to this string.
	 *
	 * This is increased when `kn_string_clone`d and decreased when
	 * `kn_string_free`d, and when it reaches zero, the struct will
	 * be freed.
	 */
	int refcount;

	/* All strings are either embedded or allocated. */
	union {
		struct {
			/* The length of the embedded string. */
			char length;

			/* The actual data for the embedded string. */
			char data[KN_STRING_EMBEDDED_LENGTH];
		} embed;

		struct {
			/*
			 * The length of the allocated string.
			 *
			 * This should equal `strlen(str)`, and is just an optimization aid.
			 */
			size_t length;

			/* The data for an allocate. */
			char *str;
		} alloc;
	};

	/*
	 * Extra padding for the struct.
	 *
	 * This is generally a number that makes this struct's size a multiple of
	 * two, but the precise length can be customized if desired.
	 */
	char _padding[KN_STRING_PADDING_LENGTH];
};

/*
 * The empty string.
 */
extern struct kn_string kn_string_empty;

/*
 * A macro to create a new embedded struct.
 *
 * It's up to the caller to ensure that `data_` can fit within an embedded
 * string.
 */
#define KN_STRING_NEW_EMBED(data_) \
	{ \
		.flags = KN_STRING_FL_EMBED, \
		.embed = { .length = sizeof(data_) - 1, .data = data_ } \
	}

/*
 * Allocates a new `kn_string` that can hold at least the given length.
 */
struct kn_string *kn_string_alloc(size_t length);

/*
 * Creates a new `kn_string` of the given length, and then initializes it to
 * `str`; the `str`'s ownership should be given given to this function.
 *
 * Note that `length` should equal `strlen(str)`.
 *
 * Also note that this will _always_ allocate strings, and never embed them.
 * (After all, a pointer's already allocated, which is what embedding is trying
 * to avoid.)
 */
struct kn_string *kn_string_new(char *str, size_t length);

/*
 * Returns the length of the string, in bytes.
 */
size_t kn_string_length(const struct kn_string *string);

/*
 * Dereferences the string, returning a mutable pointer to its data.
 */
char *kn_string_deref(struct kn_string *string);

/*
 * Duplicates this string, returning another copy of it.
 *
 * Each copy must be `kn_string_free`d separately after use to ensure that no
 * memory errors occur.
 */
struct kn_string *kn_string_clone(struct kn_string *string);

/*
 * Duplicates `KN_STRING_FL_STATIC` strings, simply returns all others.
 *
 * This is intended to be used for strings that are `static`---they normally
 * don't need to be heap-allocated (eg if they're simply being printed out), but
 * if an entire new string is needed, this is used to ensure that the returned
 * string won't change if the original static one does.
 */
struct kn_string *kn_string_clone_static(struct kn_string *string);

/*
 * Indicates that the caller is done using this string.
 *
 * For structs without the `KN_STRING_FL_EMBED` flag (ie with the `alloc` field
 * active), the refcount of the string will be decremented, and if it's zero,
 * the `str` field will be freed.
 *
 * For structs with the `KN_STRING_FL_STRUCT_ALLOC`, the entire struct itself
 * will be freed. (This is not the case when the `refcount` of an non-embedded
 * struct is not zero.)
 */
void kn_string_free(struct kn_string *string);

#endif /* !KN_STRING_H */
