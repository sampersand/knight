#include <stdio.h>  /* vfprintf, fprintf */
#include <stdarg.h> /* va_list, va_start, va_end */
#include <stdlib.h> /* exit, malloc, realloc */

#include "shared.h" /* prototypes, size_t */

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

	if (ptr == NULL)
		die("malloc failure for size %zd", size);

	return ptr;
}

void *xrealloc(void *ptr, size_t size) {
	ptr = realloc(ptr, size);

	if (ptr == NULL)
		die("realloc failure for size %zd", size);

	return ptr;
}
