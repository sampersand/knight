#include <string.h> /* strcmp, strdup */
#include <assert.h> /* assert */
#include <stdio.h>
#include "env.h"    /* prototypes, value_t, value_free */
#include "shared.h" /* xmalloc, xrealloc */

struct env_bucket_t {
	size_t capacity, length;
	variable_t *pairs;
};

#define NBUCKETS 256
static struct env_bucket_t BUCKETS[NBUCKETS];

void env_init(size_t capacity) {
	assert(capacity <= SIZE_MAX / sizeof(value_t));
	assert(capacity != 0);

	for (size_t i = 0; i < NBUCKETS; ++i)
		BUCKETS[i] = (struct env_bucket_t) {
			.capacity = capacity,
			.length = 0,
			.pairs = xmalloc(sizeof(variable_t [capacity]))
		};

}

void env_free() {
	struct env_bucket_t *bucket;

	for (size_t i = 0; i < NBUCKETS; ++i) {
		bucket = &BUCKETS[i];

		for (size_t len = 0; len < bucket->length; ++len) {
			free((char *) bucket->pairs[len].name);
			if (bucket->pairs[len].value != UNDEFINED)
				value_free(bucket->pairs[len].value);
		}

		free(bucket->pairs);
		bucket->length = 0;
		bucket->capacity = 0;
	}
}

static struct env_bucket_t *get_bucket(const char *identifier) {
	assert(identifier != NULL);

	// This is the MurmurHash.
	unsigned long hash = 525201411107845655;

	while (*identifier != '\0') {
		hash ^= *identifier++;
		hash *= 0x5bd1e9955bd1e995;
		hash ^= hash >> 47;
	}

	return &BUCKETS[hash & (NBUCKETS - 1)];
}


static variable_t *get_pair(
	const struct env_bucket_t *bucket,
	const char *identifier
) {
	for (size_t i = 0; i < bucket->length; ++i) {
		if (strcmp(bucket->pairs[i].name, identifier) == 0) {
			return &bucket->pairs[i];
		}
	}


	return NULL;
}



const char *env_name_for(value_t *value) {
	return ((variable_t *) value)->name;
}

variable_t *env_fetch(const char *identifier, bool owned) {
	struct env_bucket_t *bucket = get_bucket(identifier);
	variable_t *pair = get_pair(bucket, identifier);

	if (pair != NULL) {
		if (owned)
			free((char *) identifier);
		return pair;
	}

	if (bucket->length == bucket->capacity) {
		assert(bucket->capacity != 0);

		bucket->capacity *= 2;
		bucket->pairs = xrealloc(
			bucket->pairs,
			sizeof(variable_t [bucket->capacity])
		);
	}

	if (!owned)
		identifier = strdup(identifier);

	// note we retain ownership of the ident.
	bucket->pairs[bucket->length] = (variable_t) {
		.name = identifier,
		.value = UNDEFINED
	};

	return &bucket->pairs[bucket->length++];
}
