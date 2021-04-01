#include "string.h" /* prototypes, flags, size_t */
#include "shared.h" /* xmalloc, die, kn_hash */
#include <stdlib.h> /* fre, NULLe */
#include <string.h> /* strlen, strcmp, memcpy */
#include <assert.h> /* assert */

#ifdef KN_ARENA_ALLOCATE

/* The following two imports import these collectively:

getpagesize, mmap, munmap, MAP_FAILED, PROT_READ, PROT_WRITE, MAP_PRIVATE,
MAP_ANONYMOUS, perror
*/
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>

#ifndef KN_NUM_PAGES
# define KN_NUM_PAGES (4096*4)
#endif /* !KN_NUM_PAGES */

static struct kn_string *string_arena_start, *string_arena_next;
const struct kn_string *string_arena_end;

#define ARENASIZE (KN_NUM_PAGES * getpagesize())

void kn_string_startup() {
	string_arena_next = string_arena_start = mmap(
		NULL, ARENASIZE,
		PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS,
		-1, 0
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
	struct kn_string *curr;
	string_arena_next = NULL;

	for (curr = string_arena_start; curr != string_arena_end; ++curr) {
		assert(!(curr->flags & KN_STRING_FL_EMBED));
		assert(curr->refcount >= 0);

		if (curr->refcount)
			continue;

		if (!(curr->flags & (KN_STRING_FL_EMBED | KN_STRING_FL_STATIC)))
			free(curr->alloc.str);

		if (string_arena_next == NULL)
			string_arena_next = curr;
	}

	if (string_arena_next == NULL)
		die("Memory error: not enough strings left.");
}

static inline struct kn_string *allocate_string() {
	struct kn_string *string;

	do {
		if (string_arena_next == string_arena_end)
			free_strings();

		string = string_arena_next++;
	} while (string->refcount != 0); // mmap anon guarantees they will be zero.

	return string;
}

#else

/* Do nothing at startup for non-arena-allocated strings. */
void kn_string_startup() { }

/* Do nothing at shutdown for non-arena-allocated strings. */
void kn_string_shutdown() { }

/* Simply `xmalloc` non-arena-allocated strings. */
static inline struct kn_string *allocate_string() {
	return xmalloc(sizeof(struct kn_string));
}
#endif /* KN_ARENA_ALLOCATE */

#ifdef KN_STRING_CACHE
# ifndef KN_STRING_CACHE_MAXLEN
#  define KN_STRING_CACHE_MAXLEN 32
# endif /* !KN_STRING_CACHE_MAXLEN */
# ifndef KN_STRING_CACHE_LINESIZE
#  define KN_STRING_CACHE_LINESIZE (1<<14)
# endif /* !KN_STRING_CACHE_LINESIZE */

static struct kn_string *string_cache[
	KN_STRING_CACHE_MAXLEN][KN_STRING_CACHE_LINESIZE];

static struct kn_string **get_cache_slot(const char *str, size_t length) {
	assert(length != 0);

	unsigned long hash = kn_hash(str);

	return &string_cache[length - 1][hash & (KN_STRING_CACHE_LINESIZE - 1)];
}
#endif /* KN_STRING_CACHE */

// The empty string.
struct kn_string kn_string_empty = KN_STRING_NEW_EMBED("");

size_t kn_string_length(const struct kn_string *string) {
	return string->flags & KN_STRING_FL_EMBED
		? (size_t) string->embed.length
		: string->alloc.length;
}

char *kn_string_deref(struct kn_string *string) {
	return string->flags & KN_STRING_FL_EMBED
		? string->embed.data
		: string->alloc.str;
}

struct kn_string *kn_string_alloc(size_t length) {
	if (length == 0) 
		return &kn_string_empty;

	struct kn_string *string = allocate_string();
	string->flags = KN_STRING_FL_STRUCT_ALLOC;
	string->refcount = 1;

	if (length < KN_STRING_EMBEDDED_LENGTH) {
		string->flags |= KN_STRING_FL_EMBED;
		string->embed.length = length;
	} else {
		string->alloc.length = length;
		string->alloc.str = xmalloc(length + 1);
	}

	return string;
}

// Allocate a `kn_string` and populate it for the given `str`.
static struct kn_string *create_string(char *str, size_t length) {
	assert(strlen(str) == length);
	assert(length != 0); // should have already been checked before.

	struct kn_string *string = allocate_string();

	string->flags = KN_STRING_FL_STRUCT_ALLOC;
	string->refcount = 1;
	string->alloc.length = length;
	string->alloc.str = str;

	return string;
}

struct kn_string *kn_string_new(char *str, size_t length) {
	// sanity check for inputs.
	assert(0 <= (ssize_t) length);
	assert(str != NULL);
	assert(strlen(str) == length);

	if (length == 0) {
		free(str); // we're always given owned strings.
		return &kn_string_empty;
	}

#ifndef KN_STRING_CACHE
	return create_string(str, length);
#else
	// if it's too big just dont cache it
	// (as it's unlikely to be referenced again)
	if (KN_STRING_CACHE_MAXLEN < length)
		return create_string(str, length);

	struct kn_string **cacheline = get_cache_slot(str, length);
	struct kn_string *string;

	if (*cacheline == NULL || strcmp((string = *cacheline)->alloc.str, str))
		return *cacheline = create_string(str, length);

	free(str); // we don't need this string anymore, get rid of it.
	assert(string->refcount >= 0);

	++string->refcount;

	return string;
#endif /* !KN_STRING_CACHE */

}

void kn_string_free(struct kn_string *string) {
	assert(string != NULL);

	// if we didn't allocate the struct, simply return.
	if (!(string->flags & KN_STRING_FL_STRUCT_ALLOC)) {	
		assert(string->flags & (KN_STRING_FL_EMBED | KN_STRING_FL_STATIC));

		return;
	}

	assert(string->refcount);

	// if we aren't the last reference, then just return.
	if (--string->refcount)
		return;

#ifdef KN_STRING_CACHE
	if (string->alloc.length <= KN_STRING_CACHE_MAXLEN)
		*get_cache_slot(string->alloc.str, string->alloc.length) = 0;
#endif /* KN_STRING_CACHE */

#ifndef KN_ARENA_ALLOCATE
	// if we aren't embedded, free the allocated string.
	if (!(string->flags & KN_STRING_FL_EMBED))
		free(string->alloc.str);

	free(string);
#endif /* !KN_ARENA_ALLOCATE */

}

struct kn_string *kn_string_clone(struct kn_string *string) {
	assert(string != NULL);
	assert(!(string->flags & KN_STRING_FL_STATIC));

	++string->refcount; // this is irrelevant for non-allocated structs.

	return string;
}

struct kn_string *kn_string_clone_static(struct kn_string *string) {
	if (!(string->flags & KN_STRING_FL_STATIC))
		return string;

	size_t length = string->alloc.length;
	struct kn_string *result = kn_string_alloc(length);

	memcpy(kn_string_deref(result), string->alloc.str, length + 1);

	return result;
}
