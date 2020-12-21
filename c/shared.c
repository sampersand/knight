#include "shared.h"

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
	if (ptr == NULL) { // not `VERIFY_NOT_NULL` b/c that dies.
		bug("Attempted to free a null pointer!");
	}

	free(ptr);
}
