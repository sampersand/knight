#include "value.h"
#include "function.h"
#include "shared.h"
#include "env.h"
#include "ast.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

/*
 * 0...00000 - FALSE
 * X...X0001 - 63-bit signed integer
 * 0...00010 - NULL
 * 0...00100 - TRUE
 * X...X0000 - string (nonzero `X`)
 * X...X0010 - variable (nonzero `X`)
 * X...X0100 - function (nonzero `X`)
 * 0...01000 - undefined.
 */

#define KN_MASK 7
#define KN_TAG_STRING 0
#define KN_TAG_NUMBER 1
#define KN_TAG_VARIABLE 2
#define KN_TAG_AST 4

#define KN_TAG(x) ((x) & KN_MASK)
#define KN_UNMASK(x) ((x) & ~KN_MASK)
#define KN_VALUE_AS_NUMBER(x) ((kn_number_t) (((uint64_t) (x)) >> 1))

#ifdef DYAMIC_THEN_ARGC
#define ARITY(ast) ((ast)->argc)
#else
#define ARITY(ast) ((ast)->func->arity)
#endif

bool kn_value_is_number(kn_value_t value) {
	return value & KN_TAG_NUMBER;
}

bool kn_value_is_boolean(kn_value_t value) {
	return value == KN_FALSE || value == KN_TRUE;
}

bool kn_value_is_string(kn_value_t value) {
	return value != KN_TAG_STRING && KN_TAG(value) == KN_TAG_STRING;
}

bool kn_value_is_null(kn_value_t value) {
	return value == KN_NULL;
}

bool kn_value_is_variable(kn_value_t value) {
	return value != KN_TAG_VARIABLE && KN_TAG(value) == KN_TAG_VARIABLE;
}

bool kn_value_is_ast(kn_value_t value) {
	return value != KN_TAG_AST && KN_TAG(value) == KN_TAG_AST;
}

static bool kn_value_is_literal(kn_value_t value) {
	return value <= 4 || kn_value_is_number(value);
}

kn_number_t kn_value_as_number(kn_value_t value) {
	assert(kn_value_is_number(value));

	return ((int64_t) value) >> 1;
}

kn_boolean_t kn_value_as_boolean(kn_value_t value) {
	assert(kn_value_is_boolean(value));

	return value != KN_FALSE;
}

struct kn_string_t *kn_value_as_string(kn_value_t value) {
	assert(kn_value_is_string(value));

	return (struct kn_string_t *) value;
}

struct kn_variable_t *kn_value_as_variable(kn_value_t value) {
	assert(kn_value_is_variable(value));

	return (struct kn_variable_t *) KN_UNMASK(value);
}

struct kn_ast_t *kn_value_as_ast(kn_value_t value) {
	assert(kn_value_is_ast(value));

	return (struct kn_ast_t *) KN_UNMASK(value);
}

kn_value_t kn_value_new_number(kn_number_t number) {
	assert(number == (((number) << 1) >> 1));

	return (((uint64_t) number) << 1) | KN_TAG_NUMBER;
}

kn_value_t kn_value_new_boolean(kn_boolean_t boolean) {
	return ((uint64_t) boolean) << 2; // optimization yay
}

kn_value_t kn_value_new_string(const struct kn_string_t *string) {
	assert((uint64_t) string != KN_TAG_STRING);
	assert(KN_TAG((uint64_t) string) == 0);
	assert(string != NULL);

	return ((uint64_t) string) | KN_TAG_STRING;
}

kn_value_t kn_value_new_variable(struct kn_variable_t *value) {
	assert((uint64_t) value != KN_TAG_VARIABLE);
	assert(KN_TAG((uint64_t) value) == 0);
	assert(value != NULL);

	return ((uint64_t) value) | KN_TAG_VARIABLE;
}

kn_value_t kn_value_new_ast(const struct kn_ast_t *ast) {
	assert((uint64_t) ast != KN_TAG_AST);
	assert(KN_TAG((uint64_t) ast) == 0);
	assert(ast != NULL);

	return ((uint64_t) ast) | KN_TAG_AST;
}

static kn_number_t string_to_number(const struct kn_string_t *value) {
	kn_number_t ret = 0;
	const char *ptr = value->str;

	// strip leading whitespace.
	while (isspace(*ptr))
		ptr++;

	bool is_neg = *ptr == '-';
	unsigned char cur; // be explicit about wraparound.

	// remove leading `-` or `+`s, if they exist.
	if (is_neg || *ptr == '+')
		++ptr;

	// note that this works because of the wraparound trick.
	while ((cur = *ptr++ - '0') <= 9)
		ret = ret * 10 + cur;

	return is_neg ? -ret : ret;
}

kn_number_t kn_value_to_number(kn_value_t value) {
	assert(value != KN_UNDEFINED);

	if (kn_value_is_number(value))
		return kn_value_as_number(value);

	if (value <= KN_TRUE) {
		assert(value == KN_FALSE
			|| value == KN_NULL
			|| value == KN_TRUE);

		return value == KN_TRUE;
	}

	if (kn_value_is_string(value))
		return string_to_number(kn_value_as_string(value));

	kn_value_t ran = kn_value_run(value);
	kn_number_t ret = kn_value_to_number(ran);
	// printf(__FILE__ " %d\n", __LINE__);
	kn_value_free(ran);
	return ret;
}

kn_boolean_t kn_value_to_boolean(kn_value_t value) {
	assert(value != KN_UNDEFINED);

	if (value <= 2) {
		assert(value == KN_NULL
			|| value == KN_FALSE
			|| value == kn_value_new_number(0));
		return 0;
	}

	if (kn_value_is_number(value) || value == KN_TRUE)
		return 1;

	if (kn_value_is_string(value))
		return kn_value_as_string(value)->length != 0;

	kn_value_t ran = kn_value_run(value);
	kn_boolean_t ret = kn_value_to_boolean(ran);
	// printf(__FILE__ " %d\n", __LINE__);
	kn_value_free(ran);
	return ret;
} 

static struct kn_string_t *number_to_string(kn_number_t num) {
	// note that `22` is the length of `LONG_MIN`, which is 21 characters
	// long + the trailing `\0`.
	static char buf[22];
	// we explicitly note that refcount is `0`, as it's something that isnt
	// allocated.
	static struct kn_string_t number_string = { .refcount = 0 };

	// should have been checked earlier.
	assert(num != 0 && num != 1);

	// initialize ptr to the end of the buffer minus one, as the last is
	// the nul terminator.
	char *ptr = &buf[sizeof(buf) - 1];
	bool is_neg = num < 0;

	if (is_neg)
		num *= -1;

	do {
		*--ptr = '0' + (num % 10);
		num /= 10;
	} while (num);

	if (is_neg)
		*--ptr = '-';

	number_string.str = ptr;
	number_string.length = &buf[sizeof(buf) - 1] - ptr;

	return &number_string;
}

struct kn_string_t *kn_value_to_string(kn_value_t value) {
	static struct kn_string_t BUILTIN_STRINGS[5] = {
		{ .length = 5, .refcount = 0, .str = "false" },
		{ .length = 1, .refcount = 0, .str = "0" },
		{ .length = 4, .refcount = 0, .str = "null" },
		{ .length = 4, .refcount = 0, .str = "true" },
		{ .length = 1, .refcount = 0, .str = "1" },
	};

	assert(value != KN_UNDEFINED);

	if (value <= 4)
		return &BUILTIN_STRINGS[value];

	if (kn_value_is_number(value))
		return number_to_string(kn_value_as_number(value));

	if (kn_value_is_string(value))
		return kn_string_clone(kn_value_as_string(value));

	kn_value_t ran = kn_value_run(value);
	struct kn_string_t *ret = kn_value_to_string(ran);
	// printf(__FILE__ " %d\n", __LINE__);
	kn_value_free(ran);
	return ret;
}

void kn_value_dump(kn_value_t value) {
	assert(value != KN_UNDEFINED);

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

	switch (KN_TAG(value)) {
	case KN_TAG_STRING:
		printf("String(%s)", kn_string_deref(kn_value_as_string(value)));
		return;
	case KN_TAG_VARIABLE:
		printf("Identifier(%s)", kn_value_as_variable(value)->name);
		return;
	case KN_TAG_AST: {
		struct kn_ast_t *ast = kn_value_as_ast(value);
		printf("Function(%c", ast->func->name);

		for (size_t i = 0; i < ARITY(ast); ++i) {
			printf(", ");
			kn_value_dump(ast->args[i]);
		}

		printf(")");
		return;
	}

	default:
		assert(false);
	}
}

kn_value_t kn_value_run(kn_value_t value) {
	assert(value != KN_UNDEFINED);

	if (kn_value_is_literal(value))
		return value;

	if (KN_TAG(value) == KN_TAG_STRING) {
		(void) kn_string_clone(kn_value_as_string(value));
		return value;
	}

	if (KN_TAG(value) == KN_TAG_VARIABLE) {
		struct kn_variable_t *variable = kn_value_as_variable(value);

		if (variable->value == KN_UNDEFINED)
			die("undefined variable '%s'", variable->name);

		return kn_value_clone(variable->value);
	}

	assert(KN_TAG(value) == KN_TAG_AST);
	struct kn_ast_t *ast = kn_value_as_ast(value);

	return (ast->func->ptr)(ast->args);
}

kn_value_t kn_value_clone(kn_value_t value) {
	assert(value != KN_UNDEFINED);

	if (kn_value_is_literal(value) || KN_TAG(value) == KN_TAG_VARIABLE)
		return value;

	if (KN_TAG(value) == KN_TAG_STRING) {
		(void) kn_string_clone(kn_value_to_string(value));
		return value;
	}

	++kn_value_as_ast(value)->refcount;

	return value;
}

void kn_value_free(kn_value_t value) {
	assert(value != KN_UNDEFINED);

	if (kn_value_is_literal(value) || KN_TAG(value) == KN_TAG_VARIABLE)
		return;

	// printf("free: [%p]\n", (void *) KN_UNMASK(value));

	if (KN_TAG(value) == KN_TAG_STRING) {
		kn_string_free(kn_value_as_string(value));
		return;
	}

	struct kn_ast_t *ast = kn_value_as_ast(value);

	if (--ast->refcount)
		return;

	for (unsigned i = 0; i < ARITY(ast); ++i) {
		// printf(__FILE__ " %d\n", __LINE__);
		kn_value_free(ast->args[i]);
	}

	free(ast);
}
