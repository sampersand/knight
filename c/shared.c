#include "shared.h"

void *xmalloc(size_t size) {
	void *ret = malloc(size);

	if (ret == NULL) {
		die("malloc failure for size %zd", size);
	}

	return ret;
}

void *xrealloc(void *ptr, size_t size) {
	ptr = realloc(ptr, size);

	if (ptr == NULL) {
		die("realloc failure for size %zd", size);
	}

	return ptr;
}

void xfree(void *ptr) {
	if (ptr == NULL) {
		bug("Attempted to free a null pointer!");
	}

	free(ptr);
}
