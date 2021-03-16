#include <string.h>  /* strdup, strcmp */
#include <assert.h>  /* assert */
#include <stdlib.h>  /* free */
#include <stdbool.h> /* bool, true, false */
#include "env.h"     /* prototypes, size_t, kn_variable_t, kn_value_free */
#include "shared.h"  /* xmalloc, xrealloc, die */

// Each bucket keeps track of its own individual capacity and length.
struct kn_env_bucket_t {
	size_t capacity, length;
	struct kn_variable_t *variables;
};

#define NBUCKETS 65536
static struct kn_env_bucket_t BUCKETS[NBUCKETS];

#ifndef NDEBUG
static bool kn_env_has_been_started = false;
#endif /* NDEBUG */

void kn_env_startup(size_t capacity) {
	assert(capacity != 0);
	assert(!kn_env_has_been_started && (kn_env_has_been_started = true));

	for (size_t i = 0; i < NBUCKETS; ++i) {
		BUCKETS[i] = (struct kn_env_bucket_t) {
			.capacity = capacity,
			.length = 0,
			.variables = xmalloc(sizeof(struct kn_variable_t [capacity]))
		};
	}

}

void kn_env_shutdown() {
	struct kn_env_bucket_t *bucket;

	assert(kn_env_has_been_started && !(kn_env_has_been_started = false));

	for (size_t i = 0; i < NBUCKETS; ++i) {
		bucket = &BUCKETS[i];

		for (size_t len = 0; len < bucket->length; ++len) {
			free((char *) bucket->variables[len].name);

			if (bucket->variables[len].value != KN_UNDEFINED)
				kn_value_free(bucket->variables[len].value);
		}

		free(bucket->variables);
		bucket->length = 0;
		bucket->capacity = 0;
	}
}

static struct kn_variable_t *get_pair(
	const struct kn_env_bucket_t *bucket,
	const char *identifier
) {
	for (size_t i = 0; i < bucket->length; ++i)
		if (strcmp(bucket->variables[i].name, identifier) == 0)
			return &bucket->variables[i];

	return NULL;
}

struct kn_variable_t *kn_env_fetch(const char *identifier, bool owned) {
	unsigned long hash = kn_hash(identifier);

	struct kn_env_bucket_t *bucket = &BUCKETS[hash & (NBUCKETS - 1)];
	struct kn_variable_t *variable = get_pair(bucket, identifier);

	// If the variable exists, then simply return it.
	if (variable != NULL) {
		if (owned)
			free((char *) identifier);

		return variable;
	}

	// if the bucket is full, then we need to reallocate it.
	if (bucket->length == bucket->capacity) {
		// NOTE: that this actually causes UB, as all previous variable
		// references are then invalidated. I need to fix the alg...
		die("reallocating fails");

/*
		assert(bucket->capacity != 0);

		bucket->capacity *= 2;

		bucket->variables = xrealloc(
			bucket->variables,
			sizeof(struct kn_variable_t [bucket->capacity])
		);
*/
	}

	// Since we're making a new variable, we need ownership of the
	// identifier. So we duplicate it if we don't own it.
	if (!owned)
		identifier = strdup(identifier);


	// Create the new variable with an undefined starting value, so that any
	// attempt to access it will be invalid.
	bucket->variables[bucket->length] = (struct kn_variable_t) {
		.name = identifier,
		.value = KN_UNDEFINED
	};

	return &bucket->variables[bucket->length++];
}
