#ifndef KN_SHARED_H
#define KN_SHARED_H
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define malloc_failure(msg, ...) die("malloc failure: " msg "\n", ##__VA_ARGS__)
#define bug(msg, ...) die("bug encountered: " msg "\n", ##__VA_ARGS__)
#define die(fmt, ...) (fprintf(stderr, fmt "\n", ##__VA_ARGS__), exit(1))

void *xmalloc(size_t);
void *xrealloc(void *, size_t);
void xfree(void *);

#define VERIFY_NOT_NULL(ptr, msg, ...) \
	do { \
			if ((ptr) == NULL) { \
				malloc_failure("%s:%s:%d: null pointer encountered: " msg, __FILE__, \
					__FUNCTION__, __LINE__, ##__VA_ARGS__); \
			} \
	} while (0)

#endif
