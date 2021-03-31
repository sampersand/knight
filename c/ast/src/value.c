#include "env.h"    /* kn_variable, kn_variable_run */
#include "ast.h"    /* kn_ast, kn_ast_free, kn_ast_clone, kn_ast_run */
#include "value.h"  /* prototypes, bool, uint64_t, int64_t, kn_value, kn_number,
                       kn_boolean, KN_UNDEFINED, KN_NULL, KN_TRUE, KN_FALSE */
#include "string.h" /* kn_string, kn_string_clone, kn_string_free,
                       kn_string_deref, kn_string_length, KN_STRING_FL_STATIC,
                       KN_STRING_NEW_EMBED */
#ifndef KN_RECKLESS
#include "shared.h" /* die */
#endif /* !KN_RECKLESS */

#include <inttypes.h> /* PRId64 */
#include <stdlib.h>   /* free, NULL */
#include <assert.h>   /* assert */
#include <stdio.h>    /* printf */
#include <ctype.h>    /* isspace */

/*
 * The layout of `kn_value`:
 * 0...00000 - FALSE
 * X...XXXX1 - 63-bit signed integer
 * 0...00010 - NULL
 * 0...00100 - TRUE
 * 0...01000 - undefined.
 * X...X0000 - string (nonzero `X`)
 * X...X0010 - variable (nonzero `X`)
 * X...X0100 - function (nonzero `X`)
 * X...X0110 - custom (nonzero `X`) (only with `KN_EXT_CUSTOM_TYPES`)
 * note all pointers are 16+-bit-aligned.
 */

// note that since strings are used so frequently, casting them is a no-op.
#define KN_NUMBER_SHIFT 1
#define KN_TAG_STRING 0
#define KN_TAG_NUMBER 1
#define KN_TAG_VARIABLE 2
#define KN_TAG_AST 4

#define KN_TAG(x) ((x) & 7)
#define KN_UNMASK(x) ((x) & ~7)

bool kn_value_is_number(kn_value value) {
	return value & KN_TAG_NUMBER;
}

bool kn_value_is_boolean(kn_value value) {
	return value == KN_FALSE || value == KN_TRUE;
}

bool kn_value_is_string(kn_value value) {
	return value != KN_TAG_STRING && KN_TAG(value) == KN_TAG_STRING;
}

bool kn_value_is_variable(kn_value value) {
	return value != KN_TAG_VARIABLE && KN_TAG(value) == KN_TAG_VARIABLE;
}

bool kn_value_is_ast(kn_value value) {
	return value != KN_TAG_AST && KN_TAG(value) == KN_TAG_AST;
}

static bool kn_value_is_literal(kn_value value) {
	return value <= 4 || kn_value_is_number(value);
}

kn_number kn_value_as_number(kn_value value) {
	assert(kn_value_is_number(value));

	return ((int64_t) value) >> KN_NUMBER_SHIFT;
}

kn_boolean kn_value_as_boolean(kn_value value) {
	assert(kn_value_is_boolean(value));

	return value != KN_FALSE;
}

struct kn_string *kn_value_as_string(kn_value value) {
	assert(kn_value_is_string(value));

	return (struct kn_string *) value;
}

struct kn_variable *kn_value_as_variable(kn_value value) {
	assert(kn_value_is_variable(value));

	return (struct kn_variable *) KN_UNMASK(value);
}

struct kn_ast *kn_value_as_ast(kn_value value) {
	assert(kn_value_is_ast(value));

	return (struct kn_ast *) KN_UNMASK(value);
}

kn_value kn_value_new_number(kn_number number) {
	assert(number == ((number << KN_NUMBER_SHIFT) >> KN_NUMBER_SHIFT));

	return (((uint64_t) number) << KN_NUMBER_SHIFT) | KN_TAG_NUMBER;
}

kn_value kn_value_new_boolean(kn_boolean boolean) {
	return ((uint64_t) boolean) << 2; // micro-optimizations hooray!
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
	while (isspace(*ptr))
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

	if (kn_value_is_number(value))
		return kn_value_as_number(value);

	if (value <= KN_TRUE) {
		// sanity check
		assert(value == KN_FALSE || value == KN_NULL || value == KN_TRUE);

		return value == KN_TRUE;
	}

	if (kn_value_is_string(value))
		return string_to_number(kn_value_as_string(value));

	assert(kn_value_is_variable(value) || kn_value_is_ast(value));

	// simply execute the value and call this function again.
	kn_value ran = kn_value_run(value);
	kn_number ret = kn_value_to_number(ran);
	kn_value_free(ran);

	return ret;
}

kn_boolean kn_value_to_boolean(kn_value value) {
	assert(value != KN_UNDEFINED);

	if (value <= 4) {
		// sanity check.
		assert(value == KN_NULL
			|| value == KN_FALSE
			|| value == KN_TRUE
			|| value == kn_value_new_number(0)
			|| value == kn_value_new_number(1));

		return value >= 2;
	}

	if (kn_value_is_number(value)) {// already checked the zero case
		assert(value != kn_value_new_number(0));

		return 1;
	}

	if (kn_value_is_string(value))
		return kn_string_length(kn_value_as_string(value)) != 0;

	assert(kn_value_is_variable(value) || kn_value_is_ast(value));

	// simply execute the value and call this function again.
	kn_value ran = kn_value_run(value);
	kn_boolean ret = kn_value_to_boolean(ran);
	kn_value_free(ran);

	return ret;
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
	static struct kn_string builtin_strings[5] = {
		KN_STRING_NEW_EMBED("false"),
		KN_STRING_NEW_EMBED("0"),
		KN_STRING_NEW_EMBED("null"),
		KN_STRING_NEW_EMBED("1"),
		KN_STRING_NEW_EMBED("true"),
	};

	assert(value != KN_UNDEFINED);

	if (value <= 4)
		return &builtin_strings[value];

	if (kn_value_is_number(value))
		return number_to_string(kn_value_as_number(value));

	if (kn_value_is_string(value))
		return kn_string_clone(kn_value_as_string(value));

	assert(kn_value_is_variable(value) || kn_value_is_ast(value));

	// simply execute the value and call this function again.
	kn_value ran = kn_value_run(value);
	struct kn_string *ret = kn_value_to_string(ran);
	kn_value_free(ran);

	return ret;
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

	if (kn_value_is_number(value)) {
		printf("Number(%" PRId64 ")", kn_value_as_number(value));
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

#ifdef KN_RECKLESS
		;
#else
		die("unknown tag '%d'", KN_TAG(value));
#endif /* KN_RECKLESS */

	}
}

kn_value kn_value_run(kn_value value) {
	assert(value != KN_UNDEFINED);

	// the whole point of literals is they dont do anything when evaluated.
	if (kn_value_is_literal(value))
		return value;

	if (KN_TAG(value) == KN_TAG_VARIABLE)
		return kn_variable_run(kn_value_as_variable(value));

	// we need to create a new string, as it needs to be unique from `value`.
	if (KN_TAG(value) == KN_TAG_STRING)
		return kn_value_new_string(kn_string_clone(kn_value_as_string(value)));

	return kn_ast_run(kn_value_as_ast(value));
}

kn_value kn_value_clone(kn_value value) {
	assert(value != KN_UNDEFINED);

	// Note we don't need to clone variables, as they live for the lifetime of
	// the program.
	if (kn_value_is_literal(value) || KN_TAG(value) == KN_TAG_VARIABLE)
		return value;

	if (KN_TAG(value) == KN_TAG_STRING)
		return kn_value_new_string(kn_string_clone(kn_value_as_string(value)));

	return kn_value_new_ast(kn_ast_clone(kn_value_as_ast(value)));
}

void kn_value_free(kn_value value) {
	assert(value != KN_UNDEFINED);

	// note that variables are freed when `kn_env_free` is run.
	if (kn_value_is_literal(value) || KN_TAG(value) == KN_TAG_VARIABLE)
		return;

	if (KN_TAG(value) == KN_TAG_STRING) {
		kn_string_free(kn_value_as_string(value));
		return;
	}

	kn_ast_free(kn_value_as_ast(value));
}
