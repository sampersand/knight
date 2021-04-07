#include <stdio.h>  /* vfprintf, fprintf, stderr */
#include <stdarg.h> /* va_list, va_start, va_end */
#include <stdlib.h> /* exit, malloc, realloc, aligned_alloc */
#include <assert.h> /* assert */
#include "shared.h" /* prototypes, size_t, ssize_t, NULL */
#include "value.h"  /* KN_VALUE_ALIGN */

void die(const char *fmt, ...) {
	va_list args;

	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);

	fprintf(stderr, "\n");

	exit(1);
}

unsigned long kn_hash(const char *str) {
	unsigned long hash;

	assert(str != NULL);

	// This is the MurmurHash.
	hash = 525201411107845655;

	while (*str != '\0') {
		hash ^= *str++;
		hash *= 0x5bd1e9955bd1e995;
		hash ^= hash >> 47;
	}

	return hash;
}

void *xmalloc(size_t size) {
	assert(0 <= (ssize_t) size);

	void *ptr = malloc(size);

#ifndef KN_RECKLESS
	if (KN_UNLIKELY(ptr == NULL)) {
		fprintf(stderr, "malloc failure for size %zd", size);
		abort();
	}
#endif /* !KN_RECKLESS */

	return ptr;
}

void *xrealloc(void *ptr, size_t size) {
	assert(0 <= (ssize_t) size);

	ptr = realloc(ptr, size);

#ifndef KN_RECKLESS
	if (KN_UNLIKELY(ptr == NULL)) {
		die("realloc failure for size %zd", size);
		abort();
	}
#endif /* !KN_RECKLESS */

	return ptr;
}
