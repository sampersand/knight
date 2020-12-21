#include <stdlib.h>
#include <string.h>

#include "value.h"
#include "ast.h"
#include "shared.h"

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

struct kn_value_t kn_value_new_null(void) {
	return (struct kn_value_t) {
		.kind = KN_VT_NULL
	};
}

struct kn_string_t kn_value_to_string(const struct kn_value_t *value) {
	switch(value->kind) {
	case KN_VT_INTEGER: 
		return kn_string_from_integer(value->integer);

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

kn_integer_t kn_value_to_integer(const struct kn_value_t *value) {
	switch(value->kind) {
	case KN_VT_INTEGER:
		return value->integer;

	case KN_VT_STRING:
		return kn_string_to_integer(&value->string);

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

struct kn_value_t kn_value_add(
	const struct kn_value_t *lhs,
	const struct kn_value_t *rhs
) {
	// If lhs is a string, convert both to a string.
	if (lhs->kind == KN_VT_STRING) {
		struct kn_string_t rstring = kn_value_to_string(rhs);

		if (strlen(lhs->string.str) == 0) {
			return kn_value_new_string(rstring);
		}

		char *concat = xmalloc(
			strlen(lhs->string.str) +
			strlen(rstring.str) +
			1
		);

		strcpy(concat, lhs->string.str);
		strcat(concat, rstring.str);

		kn_string_free(&rstring);

		return kn_value_new_string(kn_string_new(concat));
	}

	// otherwise, convert both to an integer and add them.

	kn_integer_t augend = kn_value_to_integer(lhs);
	kn_integer_t addend = kn_value_to_integer(rhs);

	return kn_value_new_integer(augend + addend);
}

struct kn_value_t kn_value_sub(
	const struct kn_value_t *lhs,
	const struct kn_value_t *rhs
) {
	kn_integer_t minuend = kn_value_to_integer(lhs);
	kn_integer_t subtrahend = kn_value_to_integer(rhs);

	return kn_value_new_integer(minuend - subtrahend);
}

struct kn_value_t kn_value_mul(
	const struct kn_value_t *lhs, 
	const struct kn_value_t *rhs
) {
	// A string multiplied by a number duplicates the string that amount.
	if (lhs->kind == KN_VT_STRING) {
		size_t lhs_length = strlen(lhs->string.str);

		// if we have an empty string, return that.
		if (lhs_length == 0) {
			return kn_value_new_string(kn_string_intern(""));
		}

		kn_integer_t amnt = kn_value_to_integer(rhs);

		if (amnt == 0) {
			return kn_value_new_string(kn_string_intern(""));
		} else if (amnt == 1) {
			return kn_value_clone(lhs);
		}

		if (amnt != (kn_integer_t) (size_t) amnt) {
			die("multiplicand '%jd' is too large!", amnt);
		}

		char *string = xmalloc(1 + lhs_length * (size_t) amnt);

		for (; amnt != 0; --amnt) {
			strcat(string, lhs->string.str);
		}

		return kn_value_new_string(kn_string_new(string));
	}

	// otherwise, just convert both to integers and use that.

	kn_integer_t multiplier = kn_value_to_integer(lhs);
	kn_integer_t multiplicand = kn_value_to_integer(rhs);

	return kn_value_new_integer(multiplier * multiplicand);
}

struct kn_value_t kn_value_div(
	const struct kn_value_t *lhs, 
	const struct kn_value_t *rhs
) {
	kn_integer_t dividend = kn_value_to_integer(lhs);
	kn_integer_t divisor = kn_value_to_integer(rhs);

	if (divisor == 0) {
		die("attempted to divide by zero");
	}

	return kn_value_new_integer(dividend / divisor);
}

struct kn_value_t kn_value_mod(
	const struct kn_value_t *lhs, 
	const struct kn_value_t *rhs
) {
	kn_integer_t number = kn_value_to_integer(lhs);
	kn_integer_t base = kn_value_to_integer(rhs);

	if (base == 0) {
		die("attempted to modulo by zero");
	}

	return kn_value_new_integer(number % base);
}

// lol yay for manual impls
struct kn_value_t kn_value_pow(
	const struct kn_value_t *lhs, 
	const struct kn_value_t *rhs
) {
	kn_integer_t result = 1;
	kn_integer_t base = kn_value_to_integer(lhs);
	kn_integer_t exponent = kn_value_to_integer(rhs);

	for (; exponent != 0; --exponent) {
		result *= base;
	}

	return kn_value_new_integer(result);
}


int kn_value_cmp(const struct kn_value_t *lhs, const struct kn_value_t *rhs) {
	// return early if the pointers are idetnical.
	if (lhs == rhs) {
		return 0;
	}

	int ret;

	// comparison is based on whatever the RHS is.
	switch (lhs->kind) {
	case KN_VT_STRING: {
		struct kn_string_t rstring = kn_value_to_string(rhs);

		ret = strcmp(lhs->string.str, rstring.str);

		kn_string_free(&rstring);
		break;
	}

	case KN_VT_INTEGER: {
		kn_integer_t cmp = kn_value_to_integer(rhs);

		ret = lhs->integer == cmp ? 0 : lhs->integer < cmp ? -1 : 1;
		break;
	}

	case KN_VT_BOOLEAN: {
		kn_boolean_t rboolean = kn_value_to_boolean(rhs);

		ret = (int) ((int) lhs->boolean - (int) rboolean);
		break;
	}

	case KN_VT_NULL:
		die("can't compare NULL");

	case KN_VT_AST: {
		struct kn_value_t evaluated = kn_ast_run(lhs->ast);

		ret = kn_value_cmp(&evaluated, rhs);

		kn_value_free(&evaluated);
		break;
	}

	default:
		bug("unknown kind '%d'", lhs->kind);
	}

	return ret < 0 ? -1 : ret > 0 ? 1 : 0;
}

struct kn_value_t kn_value_clone(const struct kn_value_t *value) {
	if (value->kind == KN_VT_STRING) {
		return kn_value_new_string(kn_string_clone(&value->string));	
	} else if (value->kind == KN_VT_AST) {
		return kn_value_new_ast(kn_ast_clone(value->ast));	
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
		break;

	case KN_VT_INTEGER:
	case KN_VT_BOOLEAN:
	case KN_VT_NULL:
		break;

	default:
		bug("unknown value kind '%d'", value->kind);
	}
}
