#include "string.h"
#include "shared.h"

typedef struct kn_string_t {
	size_t rc;
	char string_start;
} kn_string_t;

inline bool is_intern(kn_string_t const *string) {
	return string->rc == 0;
}

kn_string_t *kn_string_intern(char const *string) {
	kn_string_t *ret = malloc(sizeof(size_t) + strlen(string));
	ret->rc = 0;
	memcpy(&ret->string_start, string);
	return ret;
}

kn_string_t *kn_string_malloc(char const *string) {
	kn_string_t *ret = = kn_string_intern(string);
	++(ret->rc);
	return ptr;
}

kn_string_t *kn_string_clone(kn_string_t const *string) {
	if (!is_intern(string)) {
		++string->rc;
	}
	return string;

}
char const *kn_string_deref(kn_string_t const *);
void kn_string_free(kn_string_t *);

#endif /* KN_STRING_H */
