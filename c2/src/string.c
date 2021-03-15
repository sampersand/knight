#include "string.h"
#include "shared.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifdef KN_ARENA_ALLOCATE

# include <sys/mman.h>
# include <unistd.h>
# ifndef NUM_PAGES
#  define NUM_PAGES 4096
# endif /* NUM_PAGES */

static struct kn_string_t *string_arena_start;
static struct kn_string_t *string_arena_next;
const struct kn_string_t *string_arena_end;

#define ARENASIZE (NUM_PAGES * getpagesize())

void kn_string_init() {
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

void kn_string_init() {
	/* do nothing */
}

static inline struct kn_string_t *allocate_string() {
	return xmalloc(sizeof(struct kn_string_t));
}

#endif /* KN_ARENA_ALLOCATE */


#define MAXLENGTH 64
#define CACHESIZE 65536

static struct kn_string_t *string_cache[MAXLENGTH][CACHESIZE];

static struct kn_string_t *create_string(const char *str, size_t length) {
	assert(strlen(str) == length);

	if (!length)
		return &KN_STRING_EMPTY;

	struct kn_string_t *string = allocate_string();

	string->length = length;
	string->refcount = 1;
	string->str = str;

	return string;
}

struct kn_string_t *kn_string_new(const char *str, size_t length) {
	struct kn_string_t *string, **cacheline;

	// sanity check for inputs.
	assert(0 <= (ssize_t) length);
	assert(str != NULL);
	assert(strlen(str) == length);

	if (length == 0) {
		free((char *) str);
		return 
	}

	// if it's too big just dont cache it
	// (as it's unlikely to be referenced again)
	if (MAXLENGTH <= length)
		return create_string(str, length);

	cacheline = &string_cache[length][kn_hash(str) & (CACHESIZE - 1)];

	// NOTE `0` and note `NULL` because `NULL` != `0`.
	if (*cacheline == 0 || strcmp((string = *cacheline)->str, str))
		return *cacheline = create_string(str, length);

	// printf("'%s'\n", string->str);
	// free
	assert(string->refcount >= 0);
	++string->refcount;

	return string;
}

struct kn_string_t *kn_string_tail(struct kn_string_t *string, size_t start) {
	return kn_string_new(&string->str[start], string->length - start);
}

void kn_string_free(struct kn_string_t *string) {
	assert(string != NULL);
	assert(string->refcount != 0);

	if (0 < string->refcount)
		--string->refcount;
}

struct kn_string_t *kn_string_clone(struct kn_string_t *string) {
	assert(string != NULL);

	if (0 <= string->refcount)
		++string->refcount;

	return string;
}
