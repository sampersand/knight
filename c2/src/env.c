#include <string.h> /* strcmp, strdup */
#include <assert.h> /* assert */
#include <stdio.h>
#include "env.h"    /* prototypes, kn_value_t, kn_value_free */
#include "shared.h" /* xmalloc, xrealloc */


struct kn_env_pair_t {
	char *name;
	kn_value_t value;
};

struct kn_env_bucket_t {
	size_t capacity;
	size_t length;
	struct kn_env_pair_t *pairs;
};

#define NBUCKETS 256
static struct kn_env_bucket_t BUCKETS[NBUCKETS];

void kn_env_init(size_t capacity) {
	assert(capacity <= SIZE_MAX / sizeof(kn_value_t));
	assert(capacity != 0);

	for (size_t i = 0; i < NBUCKETS; ++i) {
		BUCKETS[i] = (struct kn_env_bucket_t) {
			.capacity = capacity,
			.length = 0,
			.pairs = xmalloc(
				capacity * sizeof(struct kn_env_pair_t))
		};
	}

}

void kn_env_free() {
	struct kn_env_bucket_t *bucket;

	for (size_t i = 0; i < NBUCKETS; ++i) {
		bucket = &BUCKETS[i];

		for (size_t len = 0; len < bucket->length; ++len) {
			free(bucket->pairs[len].name);
			kn_value_free(bucket->pairs[len].value);
		}

		free(bucket->pairs);
		bucket->length = 0;
		bucket->capacity = 0;
	}
}

static struct kn_env_bucket_t *get_bucket(const char *identifier) {
	assert(identifier != NULL);
	return &BUCKETS[0];
}

static struct kn_env_pair_t *get_pair(
	const struct kn_env_bucket_t *bucket,
	const char *identifier
) {
	for (size_t i = 0; i < bucket->length; ++i) {
		if (strcmp(bucket->pairs[i].name, identifier) == 0) {
			return &bucket->pairs[i];
		}
	}

	return NULL;
}

kn_value_t kn_env_get(const char *identifier) {
	struct kn_env_bucket_t *bucket = get_bucket(identifier);
	struct kn_env_pair_t *pair = get_pair(bucket, identifier);

	if (pair == NULL) {
		die("unknown identifier '%s'", identifier);
	}

	return pair->value;
}

void kn_env_set(const char *identifier, kn_value_t value) {
	struct kn_env_bucket_t *bucket = get_bucket(identifier);
	struct kn_env_pair_t *pair = get_pair(bucket, identifier);

	if (pair != NULL) {
		kn_value_free(pair->value);
		pair->value = value;
		return;
	}

	if (bucket->length == bucket->capacity) {
		assert(bucket->capacity != 0);

		bucket->capacity *= 2;
		bucket->pairs = xrealloc(
			bucket->pairs,
			sizeof(struct kn_env_pair_t) * bucket->capacity
		);
	}

	bucket->pairs[bucket->length] = (struct kn_env_pair_t) {
		.name = strdup(identifier),
		.value = value
	};
	++bucket->length;
}
