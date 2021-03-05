#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "value.h"
#include "env.h"
#include "bytecode.h"
#include "shared.h"

/*
 * 0...00000 - FALSE
 * X...X0001 - 63-bit signed integer
 * 0...00010 - NULL
 * 0...00100 - TRUE
 * X...X0000 - string (nonzero `X`)
 * X...X0010 - identifier (nonzero `X`)
 * X...X0100 - function (nonzero `X`)
 * X...X1XXX - free value after use.
 */

#define TAG_STRING 0
#define TAG_NUMBER 1
#define TAG_VARIABLE 2
#define TAG_BLOCK 4
#define TAG(value) (((kn_value_t) (value)) & 7)
#define UNMASK(value) (((kn_value_t) (value)) & ~7)

kn_value_t kn_value_new_number(kn_number_t number) {
	return (number << 1) | TAG_NUMBER;
}

kn_value_t kn_value_new_boolean(kn_boolean_t boolean) {
	return ((kn_value_t) boolean) << 2;
}

kn_value_t kn_value_new_string(kn_string_t *string) {
	assert(string != NULL);
	assert(TAG(string) == 0);

	return ((kn_value_t) string) | TAG_STRING;
}

kn_value_t kn_value_new_variable(kn_variable_t *variable) {
	assert(variable != NULL);
	assert(TAG(variable) == 0);

	return ((kn_value_t) variable) | TAG_VARIABLE;
}

kn_value_t kn_value_new_block(kn_block_t *block) {
	assert(block != NULL);
	assert(TAG(block) == 0);

	return ((kn_value_t) block) | TAG_BLOCK;
}

bool kn_value_is_number(kn_value_t value) {
	return value & 1;
}

bool kn_value_is_boolean(kn_value_t value) {
	return !value || value == KN_TRUE;
}

bool kn_value_is_string(kn_value_t value) {
	return value > 4 && TAG(value) == TAG_STRING;
}

bool kn_value_is_variable(kn_value_t value) {
	return value > 4 && TAG(value) == TAG_VARIABLE;
}

bool kn_value_is_block(kn_value_t value) {
	return value > 4 && TAG(value) == TAG_BLOCK;
}

static bool kn_value_is_literal(kn_value_t value) {
	return value <= 4 || kn_value_is_number(value);
}

kn_number_t kn_value_as_number(kn_value_t value) {
	assert(kn_value_is_number(value));

	return (kn_number_t) (value >> 1);
}

kn_boolean_t kn_value_as_boolean(kn_value_t value) {
	assert(kn_value_is_boolean(value));

	return value >> 2;
}

kn_string_t *kn_value_as_string(kn_value_t value) {
	assert(kn_value_is_string(value));

	return (kn_string_t *) value;
}

kn_variable_t *kn_value_as_variable(kn_value_t value) {
	assert(kn_value_is_variable(value));

	return (kn_variable_t *) UNMASK(value);
}

kn_block_t *kn_value_as_block(kn_value_t value) {
	assert(kn_value_is_block(value));

	return (kn_block_t *) UNMASK(value);
}

static kn_number_t string_to_number(kn_string_t *value) {
	kn_number_t ret = 0;
	const char *ptr = value->str;

	// strip leading whitespace.
	while (*ptr == ' ' || *ptr == '\n' || *ptr == '\r' || *ptr == '\t')
		ptr++;

	bool is_neg = *ptr == '-';
	unsigned char cur; // so we get wraparound.

	if (is_neg || *ptr == '+')
		++ptr;

	while ((cur = *ptr++ - '0') <= 9)
		ret = ret * 10 + cur;

	if (is_neg)
		ret *= -1;

	return ret;
}

kn_number_t kn_value_to_number(kn_value_t value) {
	if (kn_value_is_number(value))
		return kn_value_as_number(value);

	if (value <= KN_TRUE) {
		assert(value == KN_FALSE || value == KN_NULL || value == KN_TRUE);
		return value == KN_TRUE;
	}

	if (kn_value_is_string(value))
		return string_to_number(kn_value_as_string(value));

	kn_value_t ran = kn_value_run(value);
	kn_number_t ret = kn_value_to_number(ran);
	kn_value_free(ran);
	return ret;
}

kn_boolean_t kn_value_to_boolean(kn_value_t value) {
	if (value <= KN_NULL) 
		return 0;

	if (kn_value_is_number(value) || value == KN_TRUE)
		return 1;

	if (kn_value_is_string(value))
		return kn_value_as_string(value)->length;

	kn_value_t ran = kn_value_run(value);
	kn_boolean_t boolean = kn_value_to_string(ran);
	kn_value_free(ran);
	return boolean;
}

kn_string_t *number_to_string(kn_number_t num) {
	static char buf[41]; // initialized to zero.

	char *ptr = &buf[sizeof(buf) - 1];

	if (num == 0) return &KN_STRING_ZERO;
	if (num == 1) return &KN_STRING_ONE;

	int is_neg = num < 0;

	if (is_neg)
		num *= -1;

	do {
		*--ptr = '0' + num % 10;
		num /= 10;
	} while (num);

	if (is_neg) *--ptr = '-';

	// is this correct?
	return kn_string_emplace(ptr, &buf[sizeof(buf) - 1] - ptr);
}

kn_string_t *kn_value_to_string(kn_value_t value) {
	static kn_string_t *BUILTIN_STRINGS[5] = {
		&KN_STRING_FALSE,
		&KN_STRING_ZERO,
		&KN_STRING_NULL,
		&KN_STRING_ONE,
		&KN_STRING_TRUE
	};

	if (value <= 4)
		return BUILTIN_STRINGS[value];

	if (kn_value_is_number(value))
		return number_to_string(kn_value_as_number(value));

	if (kn_value_is_string(value))
		return kn_string_clone(kn_value_as_string(value));

	kn_value_t ran = kn_value_run(value);
	kn_string_t *string = kn_value_to_string(ran);
	kn_value_free(ran);
	return string;
}

void kn_value_dump(kn_value_t value) {
	switch (value) {
	case KN_TRUE:
		printf("Boolean(true)");
		return;
	case KN_FALSE:
		printf("Boolean(false)");
		return;
	case KN_NULL:
		printf("Null()");
		return;
	}

	if (kn_value_is_number(value)) {
		printf("Number(%lld)", kn_value_as_number(value));
		return;
	}

	switch (TAG(value)) {
	case TAG_STRING:
		printf("String(%s)", kn_value_as_string(value)->str);
		return;
	case TAG_VARIABLE:
		printf("Identifier(%s)", kn_value_as_variable(value)->name);
		return;
	case TAG_BLOCK:
		printf("Function(%p)", (void *) kn_value_as_block(value));
		return;
	default:
		assert(false);
	}
}

bool kn_value_eql(kn_value_t lhs, kn_value_t rhs) {
	if (lhs == rhs)
		return true;

	if (kn_value_is_string(lhs) && kn_value_is_string(rhs)) {
		kn_string_t *lstr = kn_value_as_string(lhs);
		kn_string_t *rstr = kn_value_as_string(rhs);
		return lstr->length == rstr->length && !strcmp(lstr->str, rstr->str);
	}

	return false;
}

kn_value_t kn_value_run(kn_value_t value) {
	if (kn_value_is_literal(value))
		return value;

	if (kn_value_is_string(value)) {
		(void) kn_string_clone(kn_value_as_string(value));
		return value;
	}

	if (kn_value_is_variable(value)) {
		kn_variable_t *ret = kn_value_as_variable(value);

		if (ret->value == KN_UNDEFINED)
			die("undefined variable '%s'", ret->name);

		return kn_value_clone(ret->value);
	}

	die("todo: eval block");
	// struct kn_ast_t *ast = KN_VALUE_AS_AST(value);

	// return (ast->func->ptr)(ast->args);
}

kn_value_t kn_value_clone(kn_value_t value) {
	assert(value != KN_UNDEFINED);

	if (kn_value_is_literal(value) || kn_value_is_variable(value))
		return value;

	if (kn_value_is_variable(value)) {
		(void) kn_string_clone(kn_value_as_string(value));
		return value;
	}

	kn_value_as_block(value)->refcount++;

	return value;
}

void kn_value_free(kn_value_t value) {
	assert(value != KN_UNDEFINED);

	if (kn_value_is_literal(value) || kn_value_is_variable(value))
		return;

	if (kn_value_is_string(value)) {
		kn_string_free(kn_value_as_string(value));
		return;
	}


	kn_block_t *block = kn_value_as_block(value);

	if (--block->refcount)
		return;

	die("todo: free block");
}
