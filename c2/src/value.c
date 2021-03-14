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
 * X...X0010 - identifier (nonzero `X`)
 * X...X0100 - function (nonzero `X`)
 * 0...01000 - undefined.
 */

#define KN_MASK 7
#define KN_TAG_STRING 0
#define KN_TAG_NUMBER 1
#define KN_TAG_IDENT 2
#define KN_TAG_AST 4

#define KN_TAG(x) ((x) & KN_MASK)
#define KN_UNMASK(x) ((x) & ~KN_MASK)
#define KN_VALUE_AS_NUMBER(x) ((kn_number_t) (((uint64_t) (x)) >> 1))
#define KN_VALUE_AS_IDENT(x) ((kn_value_t *) KN_UNMASK(x))
#define KN_VALUE_AS_STRIDENT(x) KN_VALUE_AS_IDENT(x)
#define KN_VALUE_AS_AST(x) ((struct kn_ast_t *) KN_UNMASK(x))

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
	return value && KN_TAG(value) == KN_TAG_STRING;
}

bool kn_value_is_null(kn_value_t value) {
	return value == KN_NULL;
}

bool kn_value_is_identifier(kn_value_t value) {
	return value && KN_TAG(value) == KN_TAG_IDENT;
}

kn_number_t kn_value_as_number(kn_value_t value) {
	assert(kn_value_is_number(value));
	return ((int64_t) value) >> 1;
}

kn_boolean_t kn_value_as_boolean(kn_value_t value) {
	assert(kn_value_is_boolean(value));
	return value != KN_FALSE;
}

const struct kn_string_t *kn_value_as_string(kn_value_t value) {
	assert(kn_value_is_string(value));
	return (struct kn_string_t *) value;
}

kn_value_t *kn_value_as_identifier(kn_value_t value) {
	assert(kn_value_is_identifier(value));
	return (kn_value_t *) KN_UNMASK(value);
}

inline kn_value_t kn_value_new_number(kn_number_t number) {
	// assert(((uint64_t) number) == ((((uint64_t) number) << 1) >> 1));
	return (((uint64_t) number) << 1) | KN_TAG_NUMBER;
}

inline kn_value_t kn_value_new_boolean(kn_boolean_t boolean) {
	return ((uint64_t) boolean) << 2; // optimization yay
}

inline kn_value_t kn_value_new_string(const struct kn_string_t *string) {
	assert((uint64_t) string != 0);
	assert((((uint64_t) string) & KN_MASK) == 0);
	assert(string != NULL);
	return (uint64_t) string;
}

inline kn_value_t kn_value_new_identifier(kn_value_t *value) {
	assert(value != NULL);

	return ((uint64_t) value) | KN_TAG_IDENT;
}

inline kn_value_t kn_value_new_ast(const struct kn_ast_t *ast) {
	assert((uint64_t) ast != 0);
	assert((((uint64_t) ast) & KN_MASK) == 0);
	assert(ast != NULL);
	return ((uint64_t) ast) | KN_TAG_AST;
}

static kn_number_t string_to_number(const struct kn_string_t *value) {
	kn_number_t ret = 0;
	const char *ptr = kn_string_deref(value);

	// strip leading whitespace.
	while (isspace(*ptr))
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
	assert(value != KN_UNDEFINED);

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
		return *kn_string_deref(kn_value_as_string(value));

	kn_value_t ran = kn_value_run(value);
	kn_boolean_t ret = kn_value_to_boolean(ran);
	// printf(__FILE__ " %d\n", __LINE__);
	kn_value_free(ran);
	return ret;
} 

static const struct kn_string_t *number_to_string(kn_number_t num) {
	// max length is `-LONG_MAX`, which is 21 characters long.
	static char buf[22]; // initialized to zero.
	static struct kn_string_t string = { .refcount = NULL };

	// should have been checked earlier.
	assert(num != 0 && num != 1);

	char *ptr = &buf[sizeof(buf) - 1];
	bool is_neg = num < 0;

	if (is_neg)
		num *= -1;

	do {
		*--ptr = '0' + (num % 10);
		num /= 10;
	} while (num);

	if (is_neg) *--ptr = '-';

	string.str = ptr;
	string.length = &buf[sizeof(buf) - 1] - ptr;

	// is this correct?
	return &string;
}

const struct kn_string_t *kn_value_to_string(kn_value_t value) {
	static struct kn_string_t *BUILTIN_STRINGS[5] = {
		&KN_STRING_FALSE,
		&KN_STRING_ZERO,
		&KN_STRING_NULL,
		&KN_STRING_ONE,
		&KN_STRING_TRUE
	};

	assert(value != KN_UNDEFINED);

	if (value <= 4)
		return BUILTIN_STRINGS[value];

	if (kn_value_is_number(value))
		return number_to_string(kn_value_as_number(value));

	if (kn_value_is_string(value))
		return kn_value_as_string(value);

	kn_value_t ran = kn_value_run(value);
	const struct kn_string_t *ret = kn_value_to_string(ran);
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
	case KN_TAG_IDENT:
		printf("Identifier(%s)", kn_env_name_for(KN_VALUE_AS_IDENT(value)));
		return;
	case KN_TAG_AST: {
		struct kn_ast_t *ast = KN_VALUE_AS_AST(value);
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

bool kn_value_eql(kn_value_t lhs, kn_value_t rhs) {
	assert(lhs != KN_UNDEFINED);
	assert(rhs != KN_UNDEFINED);

	if (lhs == rhs) return true;

	if (kn_value_is_string(lhs) && kn_value_is_string(rhs)) {
		const struct kn_string_t *lstr = kn_value_as_string(lhs);
		const struct kn_string_t *rstr = kn_value_as_string(rhs);
		return kn_string_length(lstr) == kn_string_length(rstr) &&
			!strcmp(kn_string_deref(lstr), kn_string_deref(rstr));
	}

	assert(lhs <= 4
		|| lhs & 1
		|| kn_value_is_string(lhs)
		|| KN_TAG(lhs) == KN_TAG_AST);

	return false;
}

kn_value_t kn_value_run(kn_value_t value) {
	assert(value != KN_UNDEFINED);

	if (KN_VALUE_IS_LITERAL(value))
		return value;

	if (KN_TAG(value) == KN_TAG_STRING) {
		(void) kn_string_clone(kn_value_as_string(value));
		return value;
	}

	if (KN_TAG(value) == KN_TAG_IDENT) {
		kn_value_t *ret = KN_VALUE_AS_IDENT(value);

		if (*ret == KN_UNDEFINED)
			die("undefined variable '%s'", kn_env_name_for(ret));

		return kn_value_clone(*ret);
	}

	assert(KN_TAG(value) == KN_TAG_AST);
	struct kn_ast_t *ast = KN_VALUE_AS_AST(value);

	return (ast->func->ptr)(ast->args);
}

kn_value_t kn_value_clone(kn_value_t value) {
	assert(value != KN_UNDEFINED);

	if (KN_VALUE_IS_LITERAL(value) || KN_TAG(value) == KN_TAG_IDENT)
		return value;

	if (KN_TAG(value) == KN_TAG_STRING) {
		kn_string_clone((struct kn_string_t *) KN_UNMASK(value));
	} else {
		assert(KN_TAG(value) == KN_TAG_AST);
		++((struct kn_ast_t*) KN_UNMASK(value))->refcount;
	}

	return value;
}

void kn_value_free(kn_value_t value) {
	assert(value != KN_UNDEFINED);

	if (KN_VALUE_IS_LITERAL(value) || kn_value_is_identifier(value))
		return;

	// printf("free: [%p]\n", (void *) KN_UNMASK(value));

	if (KN_TAG(value) == KN_TAG_STRING) {
		kn_string_free((struct kn_string_t *) KN_UNMASK(value));
		return;
	}

	assert(KN_TAG(value) == KN_TAG_AST);

	// note that both `kn_ast_t` and `kn_string_t` have their refcount
	// in the same spot, with the same alignment, and the same size.
	struct kn_ast_t *ast = (struct kn_ast_t*) KN_UNMASK(value);

	if (--ast->refcount)
		return;

	for (unsigned i = 0; i < ARITY(ast); ++i) {
		// printf(__FILE__ " %d\n", __LINE__);
		kn_value_free(ast->args[i]);
	}

	free(ast);
}
