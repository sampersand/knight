#include <string.h> /* strdup, strlen */
#include <ctype.h>  /* isspace */
#include <stdio.h>  /* printf */

#include "value.h"  /* prototypes */
#include "ast.h"    /* kn_ast_t, kn_ast_run, kn_ast_free, kn_ast_clone */
#include "shared.h" /* die, bug, xmalloc */

struct kn_value_t kn_value_new_ast(struct kn_ast_t *ast) {
	return (struct kn_value_t) {
		.kind = KN_VT_AST,
		.ast = ast
	};
}

struct kn_value_t kn_value_new_string(struct kn_string_t string) {
	return (struct kn_value_t) {
		.kind = KN_VT_STRING,
		.string = string
	};
}

struct kn_value_t kn_value_new_integer(kn_integer_t integer) {
	return (struct kn_value_t) {
		.kind = KN_VT_INTEGER,
		.integer = integer
	};
}

struct kn_value_t kn_value_new_boolean(kn_boolean_t boolean) {
	return (struct kn_value_t) {
		.kind = KN_VT_BOOLEAN,
		.boolean = boolean
	};
}

struct kn_value_t kn_value_new_null() {
	return (struct kn_value_t) {
		.kind = KN_VT_NULL
	};
}

/*
 * Creates a string from the given integer.
 *
 * When done using the returned value, it should be passed to `kn_string_free`
 * to prevent memory leaks.
 */
static struct kn_string_t string_from_integer(kn_integer_t num) {
	static char buf[41]; // initialized to zero.
	int is_neg = num < 0;

	// start two back, as the last one's `\0`.
	char *ptr = &buf[sizeof(buf) - 1];

	// optimize for the case where we have a literal `0` or `1`.
	if (num == 0) {
		return kn_string_intern("0");
	} else if (num == 1) {
		return kn_string_intern("1");
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

	char *string = strdup(ptr);

	if (string == NULL) {
		die("duplicating a string failed");
	}

	return kn_string_new(string);
}

struct kn_string_t kn_value_to_string(const struct kn_value_t *value) {
	switch(value->kind) {
	case KN_VT_INTEGER: 
		return string_from_integer(value->integer);

	case KN_VT_STRING: 
		return kn_string_clone(&value->string);

	case KN_VT_BOOLEAN: 
		return kn_string_intern(value->boolean ? "true" : "false");

	case KN_VT_NULL: 
		return kn_string_intern("null");

	case KN_VT_AST: {
		struct kn_value_t evaluated = kn_ast_run(value->ast);
		struct kn_string_t ret = kn_value_to_string(&evaluated);

		kn_value_free(&evaluated);
		return ret;
	}

	default:
		bug("invalid kind encountered: %d", value->kind);
	}
}

kn_boolean_t kn_value_to_boolean(const struct kn_value_t *value) {
	switch(value->kind) {
	case KN_VT_INTEGER: 
		return value->integer;

	case KN_VT_STRING: 
		return strlen(value->string.str);

	case KN_VT_BOOLEAN: 
		return value->boolean;

	case KN_VT_NULL: 
		return 0;

	case KN_VT_AST: {
		struct kn_value_t evaluated = kn_ast_run(value->ast);
		kn_boolean_t ret = kn_value_to_boolean(&evaluated);

		kn_value_free(&evaluated);
		return ret;
	}

	default:
		bug("invalid kind encountered: %d", value->kind);
	}
}

/*
 * Converts the given string into an integer.
 *
 * Leading whitespace is stripped. If the first non-whitespace character is '-',
 * The resulting integer will be negative.  Then, characters are taken until a 
 * non-digit character is encountered, and the digits up to that point are
 * returned. If no digits are found, `0` is returned.
 *
 * This is used instead of `strtol`, as we have no way of easily knowing what
 * the size of `intmax_t` is. As such, in the interest of portability, this
 * function was written out.
 */
static kn_integer_t string_to_integer(const struct kn_string_t *string) {
	kn_integer_t ret = 0;
	const char *ptr = string->str;

	// strip leading whitespace.
	while (isspace(*ptr)) {
		ptr++;
	}

	int is_neg = *ptr == '-';
	unsigned char cur;

	if (is_neg || *ptr == '+') {
		++ptr;
	}

	while ((cur = *ptr++ - '0') <= 9) {
		ret = ret * 10 + cur;
	}

	if (is_neg) {
		ret *= -1;
	}

	return ret;
}

kn_integer_t kn_value_to_integer(const struct kn_value_t *value) {
	switch(value->kind) {
	case KN_VT_INTEGER:
		return value->integer;

	case KN_VT_STRING:
		return string_to_integer(&value->string);

	case KN_VT_BOOLEAN:
		return (kn_integer_t) value->boolean;

	case KN_VT_NULL:
		return 0;

	case KN_VT_AST: {
		struct kn_value_t evaluated = kn_ast_run(value->ast);
		kn_integer_t ret = kn_value_to_integer(&evaluated);

		kn_value_free(&evaluated);
		return ret;
	}

	default:
		bug("invalid kind encountered: %d", value->kind);
	}
}

void kn_value_dump(const struct kn_value_t *value) {
	
	switch(value->kind) {
	case KN_VT_INTEGER:
		printf("Number(%jd)", value->integer);
		break;

	case KN_VT_STRING:
		printf("String(%s)", value->string.str);
		break;

	case KN_VT_BOOLEAN:
		printf("Boolean(%s)", value->boolean ? "true" : "false");
		break;

	case KN_VT_NULL:
		printf("Null()");
		break;

	case KN_VT_AST:
		kn_ast_dump(value->ast);
		break;

	default:
		bug("invalid kind encountered: %d", value->kind);
	}
}

struct kn_value_t kn_value_clone(const struct kn_value_t *value) {
	if (value->kind == KN_VT_STRING) {
		return kn_value_new_string(kn_string_clone(&value->string));	
	} else if (value->kind == KN_VT_AST) {
		struct kn_ast_t *ast = xmalloc(sizeof(struct kn_ast_t));
		*ast = kn_ast_clone(value->ast);
		return kn_value_new_ast(ast);
	} else {
		return *value;
	}
}

void kn_value_free(struct kn_value_t *value) {
	switch(value->kind) {
	case KN_VT_STRING:
		kn_string_free(&value->string);
		break;

	case KN_VT_AST:
		kn_ast_free(value->ast);
		free(value->ast);
		break;

	case KN_VT_INTEGER:
	case KN_VT_BOOLEAN:
	case KN_VT_NULL:
		break; // you dont need to free literal values.

	default:
		bug("unknown value kind '%d'", value->kind);
	}
}
