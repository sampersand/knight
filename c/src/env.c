#include <string.h>

#include "env.h"
#include "shared.h"

/*
 * The type that holds all known variables within Knight.
 * 
 * Since all are global, there's a single static `KN_ENV` struct.
 *
 * The current implementation is quite inefficient, and implementing a hashmap
 * would be a good place for improvement.
 */
struct kn_env_t {
	size_t capacity;
	size_t length;

	const char **keys;
	struct kn_value_t *vals;
};

// The singleton value of `kn_env_t`.
static struct kn_env_t KN_ENV;

void kn_env_init(size_t capacity) {
	KN_ENV = (struct kn_env_t) {
		.length = 0,
		.capacity = capacity,
		.keys = xmalloc(capacity * sizeof(const char *)),
		.vals = xmalloc(capacity * sizeof(struct kn_value_t))
	};
}

const struct kn_value_t *kn_env_get(const char *identifier) {
	for (size_t idx = 0; idx < KN_ENV.length; ++idx) {
		if (strcmp(KN_ENV.keys[idx], identifier) == 0) {
			return &KN_ENV.vals[idx];
		}
	}

	return NULL;
}

void kn_env_set(const char *identifier, struct kn_value_t value) {
	struct kn_value_t *prev = (struct kn_value_t *) kn_env_get(identifier);

	if (prev != NULL) {
		kn_value_free(prev);
		*prev = value;
		return;
	}

	if (KN_ENV.length == KN_ENV.capacity) {
		KN_ENV.capacity *= 2;
		KN_ENV.keys = xrealloc(KN_ENV.keys,
			sizeof(const char *) * KN_ENV.capacity);
		KN_ENV.vals = xrealloc(KN_ENV.vals,
			sizeof(struct kn_value_t) * KN_ENV.capacity);
	}

	KN_ENV.keys[KN_ENV.length] = strdup(identifier);
	KN_ENV.vals[KN_ENV.length] = value;

	++KN_ENV.length;
}
