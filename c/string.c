#include "string.h"
#include "shared.h"

#include <string.h>
#include <stdbool.h>

static inline bool is_malloced(const struct kn_string_t *string) {
	return string->rc != NULL;
}

struct kn_string_t kn_string_intern(const char *str) {
	if (str == NULL) {
		bug("null pointer passed to kn_string_new");
	}

	return (struct kn_string_t) {
		.str = str,
		.rc = NULL
	};
}

struct kn_string_t kn_string_new(const char *str) {
	if (str == NULL) {
		bug("null pointer passed to kn_string_new");
	}

	unsigned *rc = xmalloc(sizeof(unsigned));
	*rc = 1;

	return (struct kn_string_t) {
		.str = str,
		.rc = rc
	};
}

struct kn_string_t kn_string_clone(const struct kn_string_t *string) {
	if (is_malloced(string)) {
		++((struct kn_string_t *) string)->rc;
	}

	return *(struct kn_string_t *) string;
}

struct kn_string_t kn_string_from_integer(kn_integer_t num) {
	static char buf[41]; // initialized to zero.
	bool is_neg = num < 0;

	// start two back, as the last one's `\0`.
	char *ptr = &buf[sizeof(buf) - 1];

	if (num == 0) {
		return kn_string_intern("0");
	}

	if (is_neg) {
		num *= -1;
	}

	while (num) {
		*--ptr = '0' + num % 10;
		num /= 10;
	}

	if (is_neg) {
		*--ptr = '-';
	}

	char *string = strdup(ptr);

	VERIFY_NOT_NULL(string, "creating a string failed.");

	return kn_string_new(string);
}

kn_integer_t kn_string_to_integer(const struct kn_string_t *string) {
	kn_integer_t ret = 0;
	const char *ptr = string->str;

	bool is_neg = *ptr == '-';
	unsigned char cur;

	if (is_neg) {
		++ptr;
	}

	while ((cur = *ptr++ - '0') <= 9) {
		ret = ret * 10 + cur;
	}

	if (is_neg) {
		ret *= -1;
	}

	return ret;
}

void kn_string_free(struct kn_string_t *string) {
	if (is_malloced(string) && --string->rc == 0) {
		xfree((char *) string->str);
	}
}
