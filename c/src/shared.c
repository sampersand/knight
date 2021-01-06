#include "shared.h"

void die(const char *fmt, ...) {
	va_list args;

	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	fprintf(stderr, "\n");

	exit(1);
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
