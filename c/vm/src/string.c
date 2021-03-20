#include "string.h"
#include "shared.h"
#include <stdio.h>
#include <string.h>
#include <string.h>
#include <assert.h>

string_t *string_tail(string_t *string, unsigned start) {
	string_t *result = xmalloc(sizeof(string_t));

	result->length = string->length - start;
	result->rc = string->rc;
	result->str = string->str + start;
	string_clone(string);

	return result;
}

string_t *string_emplace(char *str, unsigned length) {
	if (length == 0) {
		assert(strlen(str) == 0);
		return &STRING_EMPTY;
	}

	string_t *string = xmalloc(sizeof(string_t));

	string->length = length;
	string->rc = xmalloc(sizeof(unsigned));
	string->str = str;

	return string;
}

string_t *string_new(char *str) {
	return string_emplace(str, strlen(str));
}

void string_free(string_t *string) {
	if (string->rc != NULL && !--*string->rc) {
		free(string->str);
		free(string);
	}
}

string_t *string_clone(string_t *string) {
	if (string->rc != NULL)
		++*string->rc;

	return string;
}
