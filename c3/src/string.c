#include "string.h"
#include "shared.h"
#include <stdio.h>
#include <string.h>
#include <string.h>
#include <assert.h>

kn_string_t *kn_string_tail(kn_string_t *string, unsigned start) {
	kn_string_t *result = xmalloc(sizeof(kn_string_t));

	result->length = string->length - start;
	result->refcount = string->refcount;
	result->str = string->str + start;
	kn_string_clone(string);

	return result;
}

kn_string_t *kn_string_emplace(char *str, unsigned length) {
	if (length == 0) {
		assert(strlen(str) == 0);
		return &KN_STRING_EMPTY;
	}

	kn_string_t *string = xmalloc(sizeof(kn_string_t));

	string->length = length;
	string->refcount = xmalloc(sizeof(unsigned));
	string->str = str;

	return string;
}

kn_string_t *kn_string_new(char *str) {
	return kn_string_emplace(str, strlen(str));
}

void kn_string_free(kn_string_t *string) {
	if (string->refcount && !--*string->refcount) {
		free(string->str);
		free(string);
	}
}

kn_string_t *kn_string_clone(kn_string_t *string) {
	if (string->refcount)
		++*string->refcount;

	return string;
}
