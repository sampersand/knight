#include "value.h"
#include "function.h"
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
 * X...X1XXX - free value after use.
 */

#define KN_MASK 7
#define KN_TAG_STRING 0
#define KN_TAG_NUMBER 1
#define KN_TAG_IDENT 2
#define KN_TAG_AST 4
#define FREE_MEMORY 8

#define KN_TAG(x) ((x) & KN_MASK)
#define KN_UNMASK(x) ((x) & ~KN_MASK)
#define KN_VALUE_AS_NUMBER(x) ((kn_number_t) (((uint64_t) (x)) >> 1))
#define KN_VALUE_AS_IDENT(x) ((struct kn_string_t *) KN_UNMASK(x))
#define KN_VALUE_AS_STRIDENT(x) KN_VALUE_AS_IDENT(x)
#define KN_VALUE_AS_AST(x) ((struct kn_ast_t *) KN_UNMASK(x))

#ifdef FIXED_ARGC
#define ARITY(ast) ((ast)->func->arity)
#else
#define ARITY(ast) ((ast)->argc)
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

const char *kn_value_as_identifier(kn_value_t value) {
	assert(kn_value_is_identifier(value));
	return ((struct kn_string_t *) KN_UNMASK(value))->str;
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
	const char *ptr = value->str;

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
	if (value <= 2) {
		assert(value == KN_NULL
			|| value == KN_FALSE
			|| value == kn_value_new_number(0));
		return 0;
	}

	if (kn_value_is_number(value) || value == KN_TRUE)
		return 1;

	if (kn_value_is_string(value))
		return *kn_value_as_string(value)->str;

	kn_value_t ran = kn_value_run(value);
	kn_boolean_t ret = kn_value_to_boolean(ran);
	kn_value_free(ran);
	return ret;
} 

const struct kn_string_t *number_to_string(kn_number_t num) {
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

const struct kn_string_t *kn_value_to_string(kn_value_t value) {
	static struct kn_string_t *BUILTIN_STRINGS[5] = {
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
	const struct kn_string_t *ret = kn_value_to_string(ran);
	kn_value_free(ran);
	return ret;
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

	switch (KN_TAG(value)) {
	case KN_TAG_STRING:
		printf("String(%s)", kn_value_as_string(value)->str);
		return;
	case KN_TAG_IDENT:
		printf("Identifier(%s)", KN_VALUE_AS_IDENT(value)->str);
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
	if (lhs == rhs) return true;

	if (kn_value_is_string(lhs) && kn_value_is_string(rhs)) {
		const struct kn_string_t *lstr = kn_value_as_string(lhs);
		const struct kn_string_t *rstr = kn_value_as_string(rhs);
		return lstr->length == rstr->length && !strcmp(lstr->str, rstr->str);
	}

	assert(lhs <= 4 || lhs & 1
		|| kn_value_is_string(lhs) || KN_TAG(lhs) == KN_TAG_AST);
	return false;
}

kn_value_t kn_value_run(kn_value_t value) {
	if (KN_VALUE_IS_LITERAL(value))
		return value;

	if (KN_TAG(value) == KN_TAG_STRING) {
		(void) kn_string_clone(kn_value_as_string(value));
		return value;
	}

	if (KN_TAG(value) == KN_TAG_IDENT)
		return kn_env_get(KN_VALUE_AS_IDENT(value)->str);

	assert(KN_TAG(value) == KN_TAG_AST);
	struct kn_ast_t *ast = KN_VALUE_AS_AST(value);

	return (ast->func->ptr)(ast->args);
}

kn_value_t kn_value_clone(kn_value_t value) {
	if (KN_VALUE_IS_LITERAL(value))
		return value;

	if (KN_TAG(value) == KN_TAG_AST) {
		++((struct kn_ast_t*) KN_UNMASK(value))->refcount;
	} else {
		assert(KN_TAG(value) == KN_TAG_STRING || KN_TAG(value) == KN_TAG_IDENT);
		kn_string_clone((struct kn_string_t *) KN_UNMASK(value));
	}

	return value;
}

void kn_value_free(kn_value_t value) {
	if (KN_VALUE_IS_LITERAL(value))
		return;

	if (KN_TAG(value) != KN_TAG_AST) {
		kn_string_free((struct kn_string_t *) KN_UNMASK(value));
		return;
	}

	// note that both `kn_ast_t` and `kn_string_t` have their refcount
	// in the same spot, with the same alignment, and the same size.
	struct kn_ast_t *ast = (struct kn_ast_t*) KN_UNMASK(value);

	if (--ast->refcount)
		return;

	for (unsigned i = 0; i < ARITY(ast); ++i)
		kn_value_free(ast->args[i]);

	// free(ast);
}
