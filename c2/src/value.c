#include <assert.h>
#include "value.h"
#include "shared.h"
#include "string.h"
#include <string.h>
#include <ctype.h>

#ifndef NDEBUG
static void assert_kind_valid(enum value_kind_t kind) {
	assert(kind == VK_INTEGER
		|| kind == VK_BOOLEAN
		|| kind == VK_NULL
		|| kind == VK_STRING
		|| kind == VK_IDENTIFIER
		|| kind == VK_FUNCTION);
}
#endif

void value_free(struct value_t *value) {
	enum value_kind_t kind = value->kind;

	assert_kind_valid(kind);

	if (kind <= VK_NULL)
		return;

	if (value->rc == NULL) {
		assert(kind == VK_STRING);
		return;
	}

	if (--*value->rc != 0)
		return;

	free(value->rc);

	if (kind == VK_FUNCTION) {
		for (size_t i = 0; i < value->function->arity; ++i)
			value_free(&value->args[i]);
		free(value->args);
	} else {
		assert(kind == VK_IDENTIFIER || kind == VK_STRING);
		free(value->string); // which equals value->identifier
	}
}

static struct value_t value_run_nonliteral(const struct value_t *value) {
	assert(value->kind == VK_IDENTIFIER || VK_FUNCTION);

	if (value->kind == VK_FUNCTION)
		return (value->function->func)(value->args);

	die("todo: lookup identifier");
}

struct value_t value_run(const struct value_t *value) {
	assert_kind_valid(value->kind);

	if (value->kind <= VK_NULL)
		return *value;

	if (value->kind == VK_STRING) {
		++((struct value_t *) value)->rc;
		return *value;
	}

	return value_run_nonliteral(value);
}

struct value_t value_clone(const struct value_t *value) {
	assert_kind_valid(value->kind);

	if (VK_NULL < value->kind) {
		if (value->rc == NULL)
			assert(value->kind == VK_STRING);
		else
			++((struct value_t *) value)->rc;
	}

	return *value;
}

#define CONVERT_AND_RUN(function, ret_type) \
	do { \
		struct value_t ran = value_run(value); \
		ret_type result = function(value); \
		value_free(&ran); \
		return result; \
	} while(0)

number_t value_to_integer(const struct value_t *value) {
	assert_kind_valid(value->kind);

	switch (value->kind) {
	case VK_INTEGER:
		return value->integer;
	case VK_BOOLEAN:
		return value->boolean ? 1 : 0;
	case VK_NULL:
		return 0;
	case VK_STRING: {
		number_t ret = 0;
		const char *ptr = value->string;

		// strip leading whitespace.
		while (isspace(*ptr))
			ptr++;

		int is_neg = *ptr == '-';
		unsigned char cur; // so we get wraparound.

		if (is_neg || *ptr == '+')
			++ptr;

		while ((cur = *ptr++ - '0') <= 9)
			ret = ret * 10 + cur;

		if (is_neg)
			ret *= -1;

		return ret;
	}

	case VK_IDENTIFIER:
	case VK_FUNCTION:
		CONVERT_AND_RUN(value_to_integer, number_t);
	}
}


#define INTERN_STR(string) (INTERN_BIT | (size_t) (string))

static string_t STR_TRUE = "true";
static string_t STR_FALSE = "false";
static string_t STR_NULL = "null";
static string_t STR_ZERO = "0";
static string_t STR_ONE = "1";

static size_t string_from_number(number_t num) {
	static char buf[41]; // initialized to zero.

	char *ptr = &buf[sizeof(buf) - 1];

	if (num == 0) return INTERN_STR(STR_ZERO);
	if (num == 1) return INTERN_STR(STR_ONE);

	int is_neg = num < 0;

	if (is_neg) num *= -1;

	do {
		*--ptr = '0' + num % 10;
		num /= 10;
	} while (num);

	if (is_neg) *--ptr = '-';

	return (size_t) strdup(buf);
}

size_t value_to_string(const struct value_t *value) {
	assert_kind_valid(value->kind);

	switch (value->kind) {
	case VK_INTEGER:
		return string_from_number(value->integer);

	case VK_BOOLEAN:
		return INTERN_STR(value->boolean ? STR_TRUE : STR_FALSE);

	case VK_NULL:   
		return INTERN_STR(STR_NULL);
	case VK_STRING:
		return value->rc == NULL
			? INTERN_STR(value->string)
			: (size_t) value->string;

	case VK_IDENTIFIER:
	case VK_FUNCTION:
		CONVERT_AND_RUN(value_to_string, size_t);
	}
}

bool value_to_boolean(const struct value_t *value) {
	assert_kind_valid(value->kind);

	switch (value->kind) {
	case VK_INTEGER:
		return value->integer != 0;
	case VK_BOOLEAN:
		return value->boolean;
	case VK_NULL:
		return 0;
	case VK_STRING:
		return *value->string != '\0';
	case VK_IDENTIFIER:
	case VK_FUNCTION:
		CONVERT_AND_RUN(value_to_boolean, bool);
	}
}

	
