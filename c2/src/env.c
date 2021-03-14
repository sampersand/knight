#include <string.h> /* strcmp, strdup */
#include <assert.h> /* assert */
#include <stdio.h>
#include "env.h"    /* prototypes, kn_value_t, kn_value_free */
#include "shared.h" /* xmalloc, xrealloc */

struct kn_env_bucket_t {
	size_t capacity, length;
	struct kn_variable_t *variables;
};

#define NBUCKETS 256
static struct kn_env_bucket_t BUCKETS[NBUCKETS];

void kn_env_init(size_t capacity) {
	size_t i;

	assert(capacity <= SIZE_MAX / sizeof(kn_value_t));
	assert(capacity != 0);

	for (i = 0; i < NBUCKETS; ++i) {
		BUCKETS[i] = (struct kn_env_bucket_t) {
			.capacity = capacity,
			.length = 0,
			.variables =
				xmalloc(sizeof(struct kn_variable_t [capacity]))
		};
	}

}

void kn_env_free() {
	struct kn_env_bucket_t *bucket;
	size_t i, len;

	for (i = 0; i < NBUCKETS; ++i) {
		bucket = &BUCKETS[i];

		for (len = 0; len < bucket->length; ++len) {
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
	size_t i;

	for (i = 0; i < bucket->length; ++i)
		if (strcmp(bucket->variables[i].name, identifier) == 0)
			return &bucket->variables[i];

	return NULL;
}

struct kn_variable_t *kn_env_fetch(const char *identifier, bool owned) {
	struct kn_env_bucket_t *bucket;
	struct kn_variable_t *variable;
	
	// Fetch the bucket with the hash of the identifier.
	bucket = &BUCKETS[kn_hash(identifier) & (NBUCKETS - 1)];

	// Attempt to retrieve the variable from the bucket, if it exists.
	variable = get_pair(bucket, identifier);

	// If the variable exists, then simply return it.
	if (variable != NULL) {
		if (owned)
			free((char *) identifier);

		return variable;
	}

	// if the bucket is full, then we need to reallocate it.
	if (bucket->length == bucket->capacity) {
		assert(bucket->capacity != 0);

		bucket->capacity *= 2;

		bucket->variables = xrealloc(
			bucket->variables,
			sizeof(struct kn_variable_t [bucket->capacity])
		);

		// NOTE!! that this actually causes UB, as all previous variable
		// references are then invalidated. I need to fix the alg, but
		die("reallocating fails");
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
