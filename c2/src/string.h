#ifndef KN_STRING_H
#define KN_STRING_H

#include <stddef.h> /* size_t */
#include <stdint.h>

// needs to be 15, otherwise alignment of `str` may be off.
#define KN_STRING_EMBEDDED_LENGTH (sizeof(size_t) * 2 + sizeof(char *) - 1)

enum kn_string_flags_t {
	KN_STRING_FL_STRUCT_ALLOC,
	KN_STRING_FL_EMBED,
	KN_STRING_FL_STATIC,
};

/*
 * The string type in Knight.
 *
 * This struct is created via `kn_string_new`, and should be passed to 
 * `kn_string_free` when it is no longer needed.
 */
struct kn_string_t {
	enum kn_string_flags_t flags;

	union {
		struct {
			char length;
			char data[KN_STRING_EMBEDDED_LENGTH];
		} embed;

		struct {
			size_t length;
			size_t refcount;
			char *str;
		} alloc;
	};
};

/*
 * The empty string.
 */
extern struct kn_string_t kn_string_empty;

/*
 * Initializes the string allocations.
 *
 * This should be called before `kn_string_new` is run.
 */
void kn_string_startup(void);
void kn_string_shutdown(void);

#define KN_STRING_NEW_STATIC() \
	((struct kn_string_t) { \
		.flags = KN_STRING_FL_STATIC \
	})

#define KN_STRING_NEW_EMBED(data_) \
	((struct kn_string_t) { \
		.flags = KN_STRING_FL_EMBED, \
		.embed = { .length = sizeof(data_) - 1, .data = data_ } \
	})

size_t kn_string_length(const struct kn_string_t *string);
char *kn_string_deref(struct kn_string_t *string);

struct kn_string_t *kn_string_alloc(size_t length);
struct kn_string_t *kn_string_new(char *str, size_t length);

void kn_string_free(struct kn_string_t *string);
struct kn_string_t *kn_string_clone(struct kn_string_t *string);
struct kn_string_t *kn_string_clone_static(struct kn_string_t *string);

#endif
