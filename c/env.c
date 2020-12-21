#include <string.h>

#include "env.h"
#include "shared.h"

struct kn_env_t {
	size_t capacity;
	size_t length;

	const char **keys;
	struct kn_value_t *vals;
};

static struct kn_env_t KN_ENV;

void kn_env_initialize(size_t capacity) {
	KN_ENV = (struct kn_env_t) {
		.length = 0,
		.capacity = capacity,
		.keys = xmalloc(sizeof(const char *) * capacity),
		.vals = xmalloc(sizeof(struct kn_value_t *) * capacity)
	};
}

// get the value, return `NULL` if it doesnt exist
struct kn_value_t *kn_env_get(const char *identifier) {
	for (size_t idx = 0; idx < KN_ENV.length; ++idx) {
		if (strcmp(KN_ENV.keys[idx], identifier) == 0) {
			return &KN_ENV.vals[idx];
		}
	}

	return NULL;
}

// return a ref to the original one.
void kn_env_set(const char *identifier, struct kn_value_t value) {
	struct kn_value_t *prev = kn_env_get(identifier);

	if (prev != NULL) {
		*prev = value;
		return;
	}

	if (KN_ENV.length == KN_ENV.capacity) {
		KN_ENV.capacity *= 2;
		KN_ENV.keys = xrealloc(KN_ENV.keys, KN_ENV.capacity);
		KN_ENV.vals = xrealloc(KN_ENV.vals, KN_ENV.capacity);
	}

	KN_ENV.keys[KN_ENV.length] = identifier;
	KN_ENV.vals[KN_ENV.length] = value;

	++KN_ENV.length;
}
