#include "value.h"
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
 * X...X0010 - identifier (nonzero `X`)
 * X...X0100 - function (nonzero `X`)
 */

#define KN_FALSE 0
#define KN_NULL 2
#define KN_TRUE 4

#define KN_MASK 7
#define KN_TAG_STRING 0
#define KN_TAG_NUMBER 1
#define KN_TAG_IDENT 2
#define KN_TAG_AST 4

#define KN_TAG(x) ((x) & KN_MASK)
#define KN_VALUE_AS_NUMBER(x) ((kn_number_t) (((uint64_t) (x)) >> 1))
#define KN_VALUE_AS_STRING(x) ((struct kn_string_t *) (x))
#define KN_VALUE_AS_IDENT(x) ((struct kn_string_t *) ((x) & ~KN_TAG_IDENT))
#define KN_VALUE_AS_STRIDENT(x) KN_VALUE_AS_IDENT(x)
#define KN_VALUE_AS_AST(x) ((struct kn_ast_t *) ((x) & ~KN_TAG_IDENT))

#define KN_VALUE_IS_NUMBER(x) ((x) & KN_TAG_NUMBER)
#define KN_VALUE_IS_STRING(x) ((x) && KN_TAG(x) == KN_TAG_STRING)

inline kn_value_t kn_value_new_number(kn_number_t number) {
	assert(!(((uint64_t) number) >> 63));
	return (((uint64_t) number) << 1) | KN_TAG_NUMBER;
}

inline kn_value_t kn_value_new_boolean(kn_boolean_t boolean) {
	return ((uint64_t) boolean) << 2; // optimization yay
}

inline kn_value_t kn_value_new_null() {
	return KN_NULL;
}

inline kn_value_t kn_value_new_string(const struct kn_string_t *string) {
	assert((uint64_t) string != 0);
	assert((((uint64_t) string) & KN_MASK) == 0);
	assert(string != NULL);
	return (uint64_t) string;
}

inline kn_value_t kn_value_new_identifier(const struct kn_string_t *ident) {
	assert((uint64_t) ident != 0);
	assert((((uint64_t) ident) & KN_MASK) == 0);
	assert(ident != NULL);
	return ((uint64_t) ident) | KN_TAG_IDENT;
}

inline kn_value_t kn_value_new_ast(const struct kn_ast_t *ast) {
	assert((uint64_t) ast != 0);
	assert((((uint64_t) ast) & KN_MASK) == 0);
	assert(ast != NULL);
	return ((uint64_t) ast) | KN_TAG_AST;
}

static kn_number_t string_to_number(const struct kn_string_t *value) {
	kn_number_t ret = 0;
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

kn_number_t kn_value_to_number(kn_value_t value) {
	if (KN_VALUE_IS_NUMBER(value))
		return KN_VALUE_AS_NUMBER(value);

	if (value <= KN_NULL) {
		assert(value == KN_FALSE || value == KN_NULL);
		return 0;
	}

	if (value == KN_TRUE)
		return 1;
#ifdef HELPFUL
	if (!KN_VALUE_IS_STRING(value))
		die("invalid conversion of kind %d to a string", KN_TAG(value));
#else
	assert(KN_VALUE_IS_STRING(value));
#endif

	return string_to_number(KN_VALUE_AS_STRING(value));
}

kn_boolean_t kn_value_to_boolean(kn_value_t value) {
	if (value <= 2) {
		assert(value == KN_NULL || value == KN_FALSE || value == kn_value_new_number(0));
		return 0;
	}
	if (KN_VALUE_IS_NUMBER(value) || value == KN_TRUE)
		return 1;

#ifdef HELPFUL
	if (!KN_VALUE_IS_STRING(value))
		die("invalid conversion of kind %d to a string", KN_TAG(value));
#else
	assert(KN_VALUE_IS_STRING(value));
#endif

	return !*KN_VALUE_AS_STRING(value)->string;

}

struct kn_string_t *number_to_string(kn_number_t num) {
	static char buf[41]; // initialized to zero.

	char *ptr = &buf[sizeof(buf) - 1];

	if (num == 0)
		return &KN_STRING_ZERO;
	if (num == 1)
		return &KN_STRING_ONE;

	int is_neg = num < 0;

	if (is_neg) num *= -1;

	do {
		*--ptr = '0' + num % 10;
		num /= 10;
	} while (num);

	if (is_neg) *--ptr = '-';

	return kn_string_new(strdup(buf));
}

const struct kn_string_t *kn_value_to_string(kn_value_t value) {
	if (KN_VALUE_IS_NUMBER(value))
		return number_to_string(KN_VALUE_AS_NUMBER(value));

	switch (value) {
	case KN_TRUE:
		return &KN_STRING_TRUE;
	case KN_FALSE:
		return &KN_STRING_FALSE;
	case KN_NULL:
		return &KN_STRING_NULL;
	default:
#ifdef HELPFUL
		if (!KN_VALUE_IS_STRING(value))
			die("invalid conversion of kind %d to a string", KN_TAG(value));
#else
		assert(KN_VALUE_IS_STRING(value));
#endif
		return kn_string_clone(KN_VALUE_AS_STRING(value));
	}
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
	default:
		if (KN_VALUE_IS_NUMBER(value)) {
			printf("Number(%llu)", KN_VALUE_AS_NUMBER(value));
			return;
		}
		switch (KN_TAG(value)) {
		case KN_TAG_STRING:
			printf("String(%s)", KN_VALUE_AS_STRING(value)->string);
			return;
		case KN_TAG_IDENT:
			printf("Identifier(%s)", KN_VALUE_AS_IDENT(value)->string);
			return;
		case KN_TAG_AST:
			printf("todo: tag for ast\n");
			exit(1);
			// printf("Function(%c)", ((struct kn_string_t *) (value & ~KN_TAG))->ptr);
		}
	}
}

kn_value_t kn_value_clone(kn_value_t value) {
	if (value <= 4 || KN_VALUE_IS_NUMBER(value))
		return value;
	
	if (KN_TAG(value) <= 2) {
#ifdef NDEBUG
		(void) kn_string_clone(KN_VALUE_AS_STRIDENT(value));
#else
		assert(kn_string_clone(KN_VALUE_AS_STRIDENT(value))
			== KN_VALUE_AS_STRIDENT(value));
#endif
		return value;
	}

	assert(KN_TAG(value) == KN_TAG_AST);
	++KN_VALUE_AS_AST(value)->refcount;

	return value;

}

void kn_value_free(kn_value_t value) {
	if (value <= 4 || KN_VALUE_IS_NUMBER(value))
		return;

	if (KN_TAG(value) <= 2) {
		kn_string_free(KN_VALUE_AS_STRIDENT(value));
		return;
	}

	struct kn_ast_t *ast = KN_VALUE_AS_AST(value);

	if (--ast->refcount)
		return;

	for (unsigned i = 0; i < ast->func->arity; ++i)
		kn_value_free(ast->args[i]);

	free(ast->args);
}

