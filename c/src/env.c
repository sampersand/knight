#include <string.h> /* strcmp, strdup */
#include <assert.h> /* assert */
#include <stdio.h>
#include "env.h"    /* prototypes, kn_value_t, kn_value_free */
#include "shared.h" /* xmalloc, xrealloc */

/*
 * The type that holds all known variables within Knight.
 * 
 * Since all are global, there's a single static `ENV` struct.
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
static struct kn_env_t ENV;

void kn_env_init(size_t capacity) {
	assert(capacity <= SIZE_MAX / sizeof(struct kn_value_t));
	assert(capacity != 0);

	ENV = (struct kn_env_t) {
		.length = 0,
		.capacity = capacity,
		.keys = xmalloc(capacity * sizeof(const char *)),
		.vals = xmalloc(capacity * sizeof(struct kn_value_t))
	};
}

void kn_env_free() {
	for (size_t i = 0; i < ENV.length; ++i) {
		free((char *) ENV.keys[i]);
		kn_value_free(&ENV.vals[i]);
	}
	free(ENV.keys);
	free(ENV.vals);
}

const struct kn_value_t *kn_env_get(const char *identifier) {
	assert(identifier != NULL);

	for (size_t idx = 0; idx < ENV.length; ++idx) {
		if (strcmp(ENV.keys[idx], identifier) == 0) {
			return &ENV.vals[idx];
		}
	}

	fprintf(stderr, "null! %s\n", identifier);
	fflush(stdout);
	exit(19);
	return NULL;
}
const struct kn_value_t *kn_env_get1(const char *identifier) {
	assert(identifier != NULL);

	for (size_t idx = 0; idx < ENV.length; ++idx) {
		if (strcmp(ENV.keys[idx], identifier) == 0) {
			return &ENV.vals[idx];
		}
	}

	return NULL;
}

static void reallocate() {
	assert(ENV.capacity != 0);

	ENV.capacity *= 2;
	ENV.keys = xrealloc(ENV.keys, sizeof(const char *) * ENV.capacity);
	ENV.vals = xrealloc(ENV.vals, sizeof(struct kn_value_t) * ENV.capacity);
}

void kn_env_set(const char *identifier, struct kn_value_t value) {
	assert(identifier != NULL);
	struct kn_value_t *prev = (struct kn_value_t *) kn_env_get1(identifier);

	if (prev != NULL) {
		kn_value_free(prev);
		*prev = value;
		return;
	}


	if (ENV.length == ENV.capacity) {
		printf("ident: %s\n", identifier);
		reallocate();
	}

	ENV.keys[ENV.length] = strdup(identifier);
	ENV.vals[ENV.length] = value;

	++ENV.length;
}
