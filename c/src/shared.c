#include "shared.h"

void die(const char *fmt, ...) {
	va_list args;

	// don't call warn, as that is able to be toggled.
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");

	exit(1);
}

void warn(const char *fmt, ...) {
#ifndef KN_RT_NO_WARN
	va_list args;

	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
#else
	(void) fmt;
#endif
}

void *xmalloc(size_t size) {
	void *ptr = malloc(size);

	VERIFY_NOT_NULL(ptr, "malloc failure for size %zd", size);

	return ptr;
}

void *xrealloc(void *ptr, size_t size) {
	ptr = realloc(ptr, size);

	VERIFY_NOT_NULL(ptr, "realloc failure for size %zd", size);

	return ptr;
}

void xfree(void *ptr) {
	if (ptr == NULL) { // not `VERIFY_NOT_NULL` b/c that `dies`, not `bug`.
		bug("Attempted to free a null pointer!");
	}

	free(ptr);
}
