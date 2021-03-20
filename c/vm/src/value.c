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
#define TAG(value) (((value_t) (value)) & 7)
#define UNMASK(value) (((value_t) (value)) & ~7)

value_t value_new_number(number_t number) {
	return ((value_t) number << 1) | TAG_NUMBER;
}

value_t value_new_boolean(boolean_t boolean) {
	return ((value_t) boolean) << 2;
}

value_t value_new_string(string_t *string) {
	assert(string != NULL);
	assert(TAG(string) == 0);

	return ((value_t) string) | TAG_STRING;
}

value_t value_new_variable(variable_t *variable) {
	assert(variable != NULL);
	assert(TAG(variable) == 0);

	return ((value_t) variable) | TAG_VARIABLE;
}

value_t value_new_block(blockptr_t *block) {
	assert(block != NULL);
	assert(TAG(block) == 0);

	return ((value_t) block) | TAG_BLOCK;
}

bool value_is_number(value_t value) {
	return value & 1;
}

bool value_is_boolean(value_t value) {
	return !value || value == TRUE_;
}

bool value_is_string(value_t value) {
	return value > 4 && TAG(value) == TAG_STRING;
}

bool value_is_variable(value_t value) {
	return value > 4 && TAG(value) == TAG_VARIABLE;
}

bool value_is_block(value_t value) {
	return value > 4 && TAG(value) == TAG_BLOCK;
}

static bool value_is_literal(value_t value) {
	return value <= 4 || value_is_number(value);
}

number_t value_as_number(value_t value) {
	assert(value_is_number(value));

	return ((number_t) value) >> 1;
}

boolean_t value_as_boolean(value_t value) {
	assert(value_is_boolean(value));

	return value >> 2;
}

string_t *value_as_string(value_t value) {
	assert(value_is_string(value));

	return (string_t *) value;
}

variable_t *value_as_variable(value_t value) {
	assert(value_is_variable(value));

	return (variable_t *) UNMASK(value);
}

blockptr_t *value_as_block(value_t value) {
	assert(value_is_block(value));

	return (blockptr_t *) UNMASK(value);
}

static number_t string_to_number(string_t *value) {
	number_t ret = 0;
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

number_t value_to_number(value_t value) {
	if (value_is_number(value))
		return value_as_number(value);

	if (value <= TRUE_) {
		assert(value == FALSE_ || value == NULL_ || value == TRUE_);
		return value == TRUE_;
	}

	if (value_is_string(value))
		return string_to_number(value_as_string(value));

	value_t ran = value_run(value);
	number_t ret = value_to_number(ran);
	value_free(ran);
	return ret;
}

boolean_t value_to_boolean(value_t value) {
	if (value <= NULL_) 
		return 0;

	if (value_is_number(value) || value == TRUE_)
		return 1;

	if (value_is_string(value))
		return value_as_string(value)->length;

	value_t ran = value_run(value);
	boolean_t boolean = value_to_string(ran);
	value_free(ran);
	return boolean;
}

string_t *number_to_string(number_t num) {
	static char buf[41]; // initialized to zero.

	char *ptr = &buf[sizeof(buf) - 1];

	if (num == 0) return &STRING_ZERO;
	if (num == 1) return &STRING_ONE;

	int is_neg = num < 0;

	if (is_neg)
		num *= -1;

	do {
		*--ptr = '0' + num % 10;
		num /= 10;
	} while (num);

	if (is_neg) *--ptr = '-';

	// is this correct?
	return string_emplace(ptr, &buf[sizeof(buf) - 1] - ptr);
}

string_t *value_to_string(value_t value) {
	static string_t *BUILTIN_STRINGS[5] = {
		&STRING_FALSE,
		&STRING_ZERO,
		&STRING_NULL,
		&STRING_ONE,
		&STRING_TRUE
	};

	if (value <= 4)
		return BUILTIN_STRINGS[value];

	if (value_is_number(value))
		return number_to_string(value_as_number(value));

	if (value_is_string(value))
		return string_clone(value_as_string(value));

	value_t ran = value_run(value);
	string_t *string = value_to_string(ran);
	value_free(ran);
	return string;
}

void value_dump(value_t value) {
	switch (value) {
	case TRUE_:
		printf("Boolean(true)");
		return;
	case FALSE_:
		printf("Boolean(false)");
		return;
	case NULL_:
		printf("Null()");
		return;
	}

	if (value_is_number(value)) {
		printf("Number(%lli)", value_as_number(value));
		return;
	}

	switch (TAG(value)) {
	case TAG_STRING:
		printf("String(%s)", value_as_string(value)->str);
		return;
	case TAG_VARIABLE:
		printf("Identifier(%s)", value_as_variable(value)->name);
		return;
	case TAG_BLOCK:
		printf("Function(%p)", (void *) value_as_block(value));
		return;
	default:
		assert(false);
	}
}

bool value_eql(value_t lhs, value_t rhs) {
	if (lhs == rhs)
		return true;

	if (value_is_string(lhs) && value_is_string(rhs)) {
		string_t *lstr = value_as_string(lhs);
		string_t *rstr = value_as_string(rhs);
		return lstr->length == rstr->length && !strcmp(lstr->str, rstr->str);
	}

	return false;
}

value_t value_run(value_t value) {
	if (value_is_literal(value))
		return value;

	if (value_is_string(value)) {
		(void) string_clone(value_as_string(value));
		return value;
	}

	if (value_is_variable(value)) {
		variable_t *ret = value_as_variable(value);

		if (ret->value == UNDEFINED)
			die("undefined variable '%s'", ret->name);

		return value_clone(ret->value);
	}

	die("todo: eval block");
	// struct ast_t *ast = KN_VALUE_AS_AST(value);

	// return (ast->func->ptr)(ast->args);
}

value_t value_clone(value_t value) {
	assert(value != UNDEFINED);

	if (value_is_literal(value) || value_is_variable(value))
		return value;

	if (value_is_variable(value)) {
		(void) string_clone(value_as_string(value));
		return value;
	}

	value_as_block(value)->rc++;

	return value;
}

void value_free(value_t value) {
	assert(value != UNDEFINED);

	if (value_is_literal(value) || value_is_variable(value))
		return;

	if (value_is_string(value)) {
		string_free(value_as_string(value));
		return;
	}


	blockptr_t *blockptr = value_as_block(value);

	if (--blockptr->rc)
		return;

	if (!--blockptr->block->rc)
		free(blockptr->block);

	free(blockptr);
}
