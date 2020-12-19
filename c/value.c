#include "value.h"
#include "shared.h"
#include <stdlib.h>
#include <string.h>

kn_value_t kn_value_new_string(kn_string_t string) {
	return (kn_value_t) { .kind = KN_VT_STRING, .string = string };
}

kn_value_t kn_value_new_integer(kn_integer_t integer) {
	return (kn_value_t) { .kind = KN_VT_INTEGER, .integer = integer };
}

kn_value_t kn_value_new_boolean(kn_boolean_t boolean) {
	return (kn_value_t) { .kind = KN_VT_BOOLEAN, .boolean = boolean };
}

kn_value_t kn_value_new_null(void) {
	return (kn_value_t) { .kind = KN_VT_NULL };
}

// note: this uses an internal buffer.
char *kn_integer_to_string(kn_integer_t num) {
	static char buf[41]; // zero-initialized.
	bool is_neg = num < 0;
	char *ptr = &buf[sizeof(buf) - 2]; // start two back, as the last one's `\0`.

	if (!num) {
		return "0";
	}

	if (is_neg) {
		num *= -1;
	}

	while (num) {
		*--ptr = '0' + num % 10;
		num /= 10;
	}

	if (is_neg) {
		*--ptr = '-';
	}

	return ptr;
}

kn_integer_t string_to_kn_integer(const char *text) {
	char cur;
	kn_integer_t ret = 0;
	bool is_neg = *text == '-';

	if (is_neg) {
		++text;
	}

	while ((cur = *(text++) - '0') <= 9) {
		ret = ret * 10 + cur;
	}

	if (is_neg) {
		ret *= -1;
	}


	printf(" %d]\n", (int)ret);
	return ret;
}

kn_string_t kn_value_to_string(const kn_value_t *value) {
	switch(value->kind) {
		case KN_VT_INTEGER: return strdup(kn_integer_to_string(value->integer));
		case KN_VT_STRING:  return strdup(value->string);
		case KN_VT_BOOLEAN: return strdup(value->boolean ? "true" : "false");
		case KN_VT_NULL:    return strdup("null");
		default: bug("invalid kind encountered: %d", value->kind);
	}
}

kn_boolean_t kn_value_to_boolean(const kn_value_t *value) {
	switch(value->kind) {
		case KN_VT_INTEGER: return value->integer;
		case KN_VT_STRING:  return strlen(value->string);
		case KN_VT_BOOLEAN: return value->boolean;
		case KN_VT_NULL:    return 0;
		default: bug("invalid kind encountered: %d", value->kind);
	}
}

kn_integer_t kn_value_to_integer(const kn_value_t *value) {
	switch(value->kind) {
		case KN_VT_INTEGER: return value->integer;
		case KN_VT_STRING:  return string_to_kn_integer(value->string);
		case KN_VT_BOOLEAN: return value->boolean;
		case KN_VT_NULL:    return 0;
		default: bug("invalid kind encountered: %d", value->kind);
	}
}

kn_value_t kn_value_clone(const kn_value_t *value) {
	return value->kind == KN_VT_STRING ? kn_value_new_string(kn_value_to_string(value)) : *value;
}

void kn_value_free(kn_value_t *value) {
	if (value->kind == KN_VT_STRING) {
		xfree(value->string);
	}
}
