#include "string.h"
#include "shared.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifdef KN_ARENA_ALLOCATE

# include <sys/mman.h>
# include <unistd.h>
# ifndef KN_NUM_PAGES
#  define KN_NUM_PAGES (4096*4)
# endif /* KN_NUM_PAGES */

static struct kn_string_t *string_arena_start;
static struct kn_string_t *string_arena_next;
const struct kn_string_t *string_arena_end;

#define ARENASIZE (KN_NUM_PAGES * getpagesize())

void kn_string_startup() {
	string_arena_next = string_arena_start = mmap(
		NULL,
		ARENASIZE,
		PROT_READ | PROT_WRITE,
		MAP_PRIVATE | MAP_ANONYMOUS,
		-1,
		0
	);

	if (string_arena_start == MAP_FAILED)
		perror("cant allocate memory");

	string_arena_end = &string_arena_start[ARENASIZE];
}


void kn_string_shutdown() {
	if (munmap(string_arena_start, ARENASIZE) == -1)
		perror("cant unmap memory");
}

static void free_strings() {
	struct kn_string_t *curr;
	string_arena_next = NULL;

	for (curr = string_arena_start; curr != string_arena_end; ++curr) {
		assert(curr->refcount >= 0);

		if (curr->refcount)
			continue;

		free((char *) curr->str);
		free(curr);

		if (string_arena_next == NULL)
			string_arena_next = curr;
	}

	if (string_arena_next == NULL)
		die("Memory error: not enough strings left.");
}

static inline struct kn_string_t *allocate_string() {
	if (string_arena_next == string_arena_end)
		free_strings();

	struct kn_string_t *string;

	do {
		string = string_arena_next++;
	} while (string->refcount); // mmap anon gaurantees they will be zero.

	return string;

}

#else /* KN_ARENA_ALLOCATE */

void kn_string_startup() {
	/* do nothing */
}

void kn_string_shutdown() {
	/* do nothing */
}

static inline struct kn_string_t *allocate_string() {
	return xmalloc(sizeof(struct kn_string_t));
}

#endif /* KN_ARENA_ALLOCATE */


#define KN_CACHE_MAXLEN 32
#define KN_CACHE_LINESIZE (1<<14)

static struct kn_string_t *string_cache[KN_CACHE_MAXLEN][KN_CACHE_LINESIZE];

static struct kn_string_t *create_string(const char *str, size_t length) {
	assert(strlen(str) == length);
	assert(length != 0); // should have already been checked before.

	struct kn_string_t *string = allocate_string();

	string->length = length;
	string->refcount = 1;
	string->str = str;

	return string;
}

static struct kn_string_t **get_cache_slot(const char *str, size_t length) {
	assert(length != 0);

	return &string_cache[length - 1][kn_hash(str) & (KN_CACHE_LINESIZE - 1)];
}

struct kn_string_t *kn_string_new(const char *str, size_t length) {
	// sanity check for inputs.
	assert(0 <= (ssize_t) length);
	assert(str != NULL);
	assert(strlen(str) == length);

	if (length == 0) {
		free((char *) str);
		return &KN_STRING_EMPTY;
	}

	// if it's too big just dont cache it
	// (as it's unlikely to be referenced again)
	if (KN_CACHE_MAXLEN < length)
		return create_string(str, length);

	struct kn_string_t **cacheline = get_cache_slot(str, length);
	struct kn_string_t *string;

	if (*cacheline == NULL || strcmp((string = *cacheline)->str, str))
		return *cacheline = create_string(str, length);

	free((char *) str); // we don't need this string anymore, get rid of it.
	assert(string->refcount >= 0);

	++string->refcount;

	return string;
}

struct kn_string_t *kn_string_tail(struct kn_string_t *string, size_t start) {
	return kn_string_new(strdup(&string->str[start]), string->length - start);
}

void kn_string_free(struct kn_string_t *string) {
	assert(string != NULL);
	// assert(string->refcount != 0);

	if (0 < string->refcount) {
		--string->refcount;

#ifndef KN_ARENA_ALLOCATE
		if (!string->refcount) {
			if (string->length <= KN_CACHE_MAXLEN)
				*get_cache_slot(string->str, string->length) = 0;
			free((char *) string->str);
			free(string);
		}
#endif /* KN_ARENA_ALLOCATE */

	}
}

struct kn_string_t *kn_string_clone(struct kn_string_t *string) {
	assert(string != NULL);

	if (0 <= string->refcount)
		++string->refcount;

	return string;
}

#ifndef NDEBUG
_Bool kn_string_is_interned(const struct kn_string_t* string) {
	return string->refcount < 0;
}
#endif /* NDEBUG */
