#include "env.h"    /* kn_variable, kn_variable_run */
#include "ast.h"    /* kn_ast, kn_ast_free, kn_ast_clone, kn_ast_run */
#include "value.h"  /* prototypes, bool, uint64_t, int64_t, kn_value, kn_number,
                       kn_boolean, KN_UNDEFINED, KN_NULL, KN_TRUE, KN_FALSE */
#include "string.h" /* kn_string, kn_string_clone, kn_string_free,
                       kn_string_deref, kn_string_length, KN_STRING_FL_STATIC,
                       KN_STRING_NEW_EMBED */
#include "shared.h" /* die */
#ifndef KN_RECKLESS
#endif /* !KN_RECKLESS */

#include <inttypes.h> /* PRId64 */
#include <stdlib.h>   /* free, NULL */
#include <assert.h>   /* assert */
#include <stdio.h>    /* printf */
#include <ctype.h>    /* isspace */

/*
 * The layout of `kn_value`:
 * 0...00000 - FALSE
 * 0...01000 - NULL
 * 0...10000 - TRUE
 * 0...11000 - undefined.
 * X...XX001 - 61-bit signed integer
 * X...XX010 - variable (nonzero `X`)
 * X...XX011 - string (nonzero `X`)
 * X...XX100 - function (nonzero `X`)
 * note all pointers are 16+-bit-aligned.
 */

kn_number kn_value_as_number(kn_value value) {
	assert(KN_VALUE_IS_NUMBER(value));

	return ((int64_t) value) >> KN_SHIFT;
}

kn_boolean kn_value_as_boolean(kn_value value) {
	assert(value == KN_TRUE || value == KN_FALSE);

	return value != KN_FALSE;
}

struct kn_string *kn_value_as_string(kn_value value) {
	assert(KN_VALUE_IS_STRING(value));

	return (struct kn_string *) KN_UNMASK(value);
}

struct kn_variable *kn_value_as_variable(kn_value value) {
	assert(KN_VALUE_IS_VARIABLE(value));

	return (struct kn_variable *) KN_UNMASK(value);
}

struct kn_ast *kn_value_as_ast(kn_value value) {
	assert(KN_VALUE_IS_AST(value));

	return (struct kn_ast *) KN_UNMASK(value);
}

kn_value kn_value_new_number(kn_number number) {
	assert(number == ((number << KN_SHIFT) >> KN_SHIFT));

	return (((uint64_t) number) << KN_SHIFT) | KN_TAG_NUMBER;
}

kn_value kn_value_new_boolean(kn_boolean boolean) {
	// return ((uint64_t) boolean) << 2; // micro-optimizations hooray!
	return boolean ? KN_TRUE : KN_FALSE;
}

kn_value kn_value_new_string(struct kn_string *string) {
	assert(string != NULL);

 	// a nonzero tag indicates a misaligned pointer
	assert(KN_TAG((uint64_t) string) == 0);

	return ((uint64_t) string) | KN_TAG_STRING;
}

kn_value kn_value_new_variable(struct kn_variable *value) {
	assert(value != NULL);

 	// a nonzero tag indicates a misaligned pointer
	assert(KN_TAG((uint64_t) value) == 0);

	return ((uint64_t) value) | KN_TAG_VARIABLE;
}

kn_value kn_value_new_ast(struct kn_ast *ast) {
	assert(ast != NULL);

 	// a nonzero tag indicates a misaligned pointer
	assert(KN_TAG((uint64_t) ast) == 0);

	return ((uint64_t) ast) | KN_TAG_AST;
}

/*
 * Convert a string to a number, as per the knight specs.
 *
 * This means we strip all leading whitespace, and then an optional `-` or `+`
 * may appear (`+` is ignored, `-` indicates a negative number). Then as many
 * digits as possible are read.
 *
 * Note that we can't use `strtoll`, as we can't be positive that `kn_number`
 * is actually a `long long`.
 */
static kn_number string_to_number(struct kn_string *string) {
	kn_number ret = 0;
	const char *ptr = kn_string_deref(string);

	// strip leading whitespace.
	while (KN_UNLIKELY(isspace(*ptr)))
		ptr++;

	bool is_neg = *ptr == '-';

	// remove leading `-` or `+`s, if they exist.
	if (is_neg || *ptr == '+')
		++ptr;

	// only digits are `<= 9` when a literal `0` char is subtracted from them.
	unsigned char cur; // be explicit about wraparound.
	while ((cur = *ptr++ - '0') <= 9)
		ret = ret * 10 + cur;

	return is_neg ? -ret : ret;
}

kn_number kn_value_to_number(kn_value value) {
	assert(value != KN_UNDEFINED);

	switch (KN_EXPECT(KN_TAG(value), KN_TAG_NUMBER)) {
	case KN_TAG_NUMBER:
		return kn_value_as_number(value);

	case KN_TAG_CONSTANT:
		return value == KN_TRUE;

	case KN_TAG_STRING:
		return string_to_number(kn_value_as_string(value));

	case KN_TAG_VARIABLE:
	case KN_TAG_AST: {
		// simply execute the value and call this function again.
		kn_value ran = kn_value_run(value);
		kn_number ret = kn_value_to_number(ran);
		kn_value_free(ran);
		return ret;
	}

	default:
		KN_UNREACHABLE();
	}
}

kn_boolean kn_value_to_boolean(kn_value value) {
	assert(value != KN_UNDEFINED);

	switch (KN_TAG(value)) {
	case KN_TAG_CONSTANT:
		return value == KN_TRUE;

	case KN_TAG_NUMBER:
		return value != KN_TAG_NUMBER;

	case KN_TAG_STRING:
		return kn_string_length(kn_value_as_string(value)) != 0;

	case KN_TAG_AST:
	case KN_TAG_VARIABLE: {
		// simply execute the value and call this function again.
		kn_value ran = kn_value_run(value);
		kn_boolean ret = kn_value_to_boolean(ran);
		kn_value_free(ran);

		return ret;
	}

	default:
		KN_UNREACHABLE();
	}
}

static struct kn_string *number_to_string(kn_number num) {
	// note that `22` is the length of `-UINT64_MIN`, which is 21 characters
	// long + the trailing `\0`.
	static char buf[22];
	static struct kn_string number_string = { .flags = KN_STRING_FL_STATIC };

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
	} while (num /= 10);

	if (is_neg)
		*--ptr = '-';

	number_string.alloc.str = ptr;
	number_string.alloc.length = &buf[sizeof(buf) - 1] - ptr;

	return &number_string;
}

struct kn_string *kn_value_to_string(kn_value value) {
	// static, embedded strings so we don't have to allocate for known strings.
	static struct kn_string builtin_strings[KN_TRUE + 1] = {
		[KN_FALSE] = KN_STRING_NEW_EMBED("false"),
		[KN_TAG_NUMBER] = KN_STRING_NEW_EMBED("0"),
		[KN_NULL] = KN_STRING_NEW_EMBED("null"),
		[KN_TRUE] = KN_STRING_NEW_EMBED("true"),
		[(((uint64_t) 1) << KN_SHIFT) | KN_TAG_NUMBER] = KN_STRING_NEW_EMBED("1"),
	};

	assert(value != KN_UNDEFINED);

	if (KN_UNLIKELY(value <= KN_TRUE))
		return &builtin_strings[value];

	switch (KN_EXPECT(KN_TAG(value), KN_TAG_STRING)) {
	case KN_TAG_NUMBER:
		return number_to_string(kn_value_as_number(value));

	case KN_TAG_STRING:
		return kn_string_clone(kn_value_as_string(value));

	case KN_TAG_AST:
	case KN_TAG_VARIABLE: {
		// simply execute the value and call this function again.
		kn_value ran = kn_value_run(value);
		struct kn_string *ret = kn_value_to_string(ran);
		kn_value_free(ran);

		return ret;
	}

	case KN_TAG_CONSTANT:
	default:
		KN_UNREACHABLE();
	}
}

void kn_value_dump(kn_value value) {
	switch (value) {
	case KN_UNDEFINED:
		printf("<KN_UNDEFINED>"); // we actually dump undefined for debugging.
		return;
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

	switch (KN_TAG(value)) {
	case KN_TAG_NUMBER:
		printf("Number(%" PRId64 ")", kn_value_as_number(value));
		return;

	case KN_TAG_STRING:
		printf("String(%s)", kn_string_deref(kn_value_as_string(value)));
		return;

	case KN_TAG_VARIABLE:
		printf("Identifier(%s)", kn_value_as_variable(value)->name);
		return;

	case KN_TAG_AST: {
		struct kn_ast *ast = kn_value_as_ast(value);

		printf("Function(%c", ast->func->name);

		for (size_t i = 0; i < ast->func->arity; ++i) {
			printf(", ");
			kn_value_dump(ast->args[i]);
		}

		printf(")");
		return;
	}

	default:
		KN_UNREACHABLE();
	}
}

kn_value kn_value_run(kn_value value) {
	assert(value != KN_UNDEFINED);

	switch (KN_EXPECT(KN_TAG(value), KN_TAG_AST)) {
	case KN_TAG_AST:
		return kn_ast_run(kn_value_as_ast(value));

	case KN_TAG_STRING:
		return kn_value_new_string(kn_string_clone(kn_value_as_string(value)));

	case KN_TAG_VARIABLE:
		return kn_variable_run(kn_value_as_variable(value));

	case KN_TAG_NUMBER:
	case KN_TAG_CONSTANT:
		return value;

	default:
		KN_UNREACHABLE();
	}
}

kn_value kn_value_clone(kn_value value) {
	assert(value != KN_UNDEFINED);

	switch (KN_EXPECT(KN_TAG(value), KN_TAG_STRING)) {
	case KN_TAG_CONSTANT:
	case KN_TAG_NUMBER:
	case KN_TAG_VARIABLE:
		// Note we don't need to clone variables, as they live for the lifetime
		// of the program.
		return value;

	case KN_TAG_AST:
		return kn_value_new_ast(kn_ast_clone(kn_value_as_ast(value)));

	case KN_TAG_STRING:
		return kn_value_new_string(kn_string_clone(kn_value_as_string(value)));

	default:
		KN_UNREACHABLE();
	}
}

void kn_value_free(kn_value value) {
	assert(value != KN_UNDEFINED);

	switch (KN_EXPECT(KN_TAG(value), KN_TAG_STRING)) {
	case KN_TAG_CONSTANT:
	case KN_TAG_NUMBER:
	case KN_TAG_VARIABLE:
		// variables are freed when `kn_env_free` is run.
		return;

	case KN_TAG_STRING:
		kn_string_free(kn_value_as_string(value));
		return;

	case KN_TAG_AST:
		kn_ast_free(kn_value_as_ast(value));
		return;

	default:
		KN_UNREACHABLE();
	}
}
