#ifndef STRING_H
#define STRING_H

#include <stdlib.h>

typedef struct {
	unsigned length, *rc;
	char *str;
} string_t;

static string_t STRING_TRUE = { 4, NULL, "true" };
static string_t STRING_FALSE = { 5, NULL, "false" };
static string_t STRING_NULL = { 4, NULL, "null" };
static string_t STRING_EMPTY = { 0, NULL, "" };
static string_t STRING_ZERO = { 1, NULL, "0" };
static string_t STRING_ONE = { 1, NULL, "1" };

string_t *string_tail(string_t *, unsigned);
string_t *string_new(char *);
string_t *string_emplace(char *, unsigned);

void string_free(string_t *);
string_t *string_clone(string_t *);

#endif
