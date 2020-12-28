#include "shared.h"

void die(const char *fmt, ...) {
	va_list args;

	warn(fmt, args);

	exit(1);
}

void warn(const char *fmt, ...) {
	va_list args;

	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
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
