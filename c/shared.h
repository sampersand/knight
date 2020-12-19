#ifndef KN_SHARED_H
#define KN_SHARED_H
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

void *xmalloc(size_t);
void *xrealloc(void *, size_t);
void xfree(void *);

#define bug(msg, ...) die("bug encountered: " msg, ##__VA_ARGS__)
#define die(...) (fprintf(stderr, ##__VA_ARGS__), exit(1))

#endif
