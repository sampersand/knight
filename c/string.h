#ifndef KN_STRING_H
#define KN_STRING_H
#include <stdint.h>

struct kn_string_t {
	const char *str;
	unsigned *rc;
};

typedef intmax_t kn_integer_t;

struct kn_string_t kn_string_intern(const char *);
struct kn_string_t kn_string_new(const char *);
struct kn_string_t kn_string_clone(const struct kn_string_t *);
struct kn_string_t kn_string_from_integer(kn_integer_t);
kn_integer_t kn_string_to_integer(const struct kn_string_t *);

void kn_string_free(struct kn_string_t *);

#endif /* KN_STRING_H */
