/*
 * The environment of Knight is used to keep track of all the variables
 * within it.
 *
 * Instead of a naive approach of simply having variables keep track of their
 * names, and then looking up the corresponding variable on each execution, we
 * use the fact that all variables are globally allocated. As such, whenever
 * an unknown identifier is given to `kn_env_fetch`, we actually make a new
 * entry for it and assign it the undefined value `KN_UNDEFINED`.
 *
 * Therefore, whenever a variable is referenced in the source code, it's given
 * an entry, even if it's never actually assigned.
 */

#include <string.h>  /* strdup, strcmp */
#include <assert.h>  /* assert */
#include <stdlib.h>  /* free */
#include <stdbool.h> /* bool, true, false */
#include "env.h"     /* prototypes, size_t, kn_value_free , kn_value*/
#include "shared.h"  /* xmalloc, xrealloc, die */

/*
 * The amount of buckets that the `kn_env_map` will have.
 *
 * The greater the number, the fewer cache collisions, but the more memory used.
 */
#ifndef KN_ENV_NBUCKETS
# define KN_ENV_NBUCKETS 65536
#endif /* !KN_ENV_NBUCKETS */

/*
 * The capacity of each bucket.
 *
 * Once this many variables are in a single bucket, the program will have to
 * reallocate those buckets.
 */
#ifndef KN_ENV_CAPACITY
# define KN_ENV_CAPACITY 256
#endif /* !KN_ENV_CAPACITY */

/*
 * The buckets of the environment hashmap.
 *
 * Each bucket keeps track of its own individual capacity and length, so they
 * can be resized separately.
 */
struct kn_env_bucket {
	size_t capacity, length;
	struct kn_variable *variables;
};

/* The mapping of all variables within Knight. */
static struct kn_env_bucket kn_env_map[KN_ENV_NBUCKETS];

#ifndef NDEBUG
/* A sanity check to ensure the environment has been setup. */
static bool kn_env_has_been_started = false;
#endif /* !NDEBUG */

void kn_env_startup() {
	// make sure we haven't started, and then set started to true.
	assert(!kn_env_has_been_started && (kn_env_has_been_started = true));
	assert(KN_ENV_CAPACITY != 0);

	for (size_t i = 0; i < KN_ENV_NBUCKETS; ++i) {
		kn_env_map[i] = (struct kn_env_bucket) {
			// technically redundant, b/c it's set to 0 in `kn_env_shutdown`.
			.length = 0,
			.capacity = KN_ENV_CAPACITY,
			.variables = xmalloc(sizeof(struct kn_variable [KN_ENV_CAPACITY]))
		};
	}
}

void kn_env_shutdown() {
	// make sure we've started, and then indicate we've shut down.
	assert(kn_env_has_been_started && !(kn_env_has_been_started = false));

	for (size_t i = 0; i < KN_ENV_NBUCKETS; ++i) {
		struct kn_env_bucket *bucket = &kn_env_map[i];

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

struct kn_variable *kn_env_fetch(const char *identifier, bool owned) {
	struct kn_env_bucket *bucket;
	struct kn_variable *variable;

	assert(identifier != NULL);

	bucket = &kn_env_map[kn_hash(identifier) & (KN_ENV_NBUCKETS - 1)];

	for (size_t i = 0; i < bucket->length; ++i) {
		variable = &bucket->variables[i];

		// if the variable already exists, return it.
		if (strcmp(variable->name, identifier) == 0) {
			if (owned)
				free((char *) identifier);

			return variable;
		}
	}

	// now we don't own it.

	// if the bucket is full, then we need to reallocate it.
	if (bucket->length == bucket->capacity) {
		// NOTE: that this actually causes UB, as all previous variable
		// references are then invalidated. There's a somewhat easy fix to this
		// via allocating contiguous kn_env_map, but ive never gotten to th
		// point where this is actually necessary.
		die("reallocating fails");

/*
		assert(bucket->capacity != 0);

		bucket->capacity *= 2;

		bucket->variables = xrealloc(
			bucket->variables,
			sizeof(struct kn_variable [bucket->capacity])
		);
*/
	}

	// Since we're making a new variable, we need ownership of the
	// identifier. So we duplicate it if we don't own it.
	if (!owned)
		identifier = strdup(identifier);

	variable = &bucket->variables[bucket->length++];
	variable->name = identifier;

	// Create the new variable with an undefined starting value, so that any
	// attempt to access it will be invalid.
	variable->value = KN_UNDEFINED;

	return variable;
}

void kn_variable_assign(struct kn_variable *variable, kn_value value) {
	if (variable->value != KN_UNDEFINED)
		kn_value_free(variable->value);

	variable->value = value;
}

const char *kn_variable_name(const struct kn_variable *variable) {
	return variable->name;
}

kn_value kn_variable_run(struct kn_variable *variable) {
#ifndef KN_RECKLESS
	if (variable->value == KN_UNDEFINED)
		die("undefined variable '%s'", kn_variable_name(variable));
#endif /*! KN_RECKLESS */

	return kn_value_clone(variable->value);
}
