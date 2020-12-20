#include "value.h"
#include "ast.h"
#include "shared.h"
#include <stdlib.h>
#include <string.h>

kn_value_t kn_value_new_ast(kn_ast_t *ast) {
	return (kn_value_t) { .kind = KN_VT_AST, .ast = ast };
}

kn_value_t kn_value_new_string(kn_string_t string) {
	return (kn_value_t) { .kind = KN_VT_STRING, .string = string };
}

kn_value_t kn_value_new_integer(kn_integer_t integer) {
	return (kn_value_t) { .kind = KN_VT_INTEGER, .integer = integer };
}

kn_value_t kn_value_new_boolean(kn_boolean_t boolean) {
	return (kn_value_t) { .kind = KN_VT_BOOLEAN, .boolean = boolean };
}

kn_value_t kn_value_new_null(void) {
	return (kn_value_t) { .kind = KN_VT_NULL };
}

// note: this uses an internal buffer.
char *kn_integer_to_string(kn_integer_t num) {
	static char buf[41]; // zero-initialized.
	bool is_neg = num < 0;
	char *ptr = &buf[sizeof(buf) - 2]; // start two back, as the last one's `\0`.

	if (!num) {
		return "0";
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

	return ptr;
}

kn_integer_t string_to_kn_integer(const char *text) {
	char cur;
	kn_integer_t ret = 0;
	bool is_neg = *text == '-';

	if (is_neg) {
		++text;
	}

	while ((cur = *(text++) - '0') <= 9) {
		ret = ret * 10 + cur;
	}

	if (is_neg) {
		ret *= -1;
	}


	printf(" %d]\n", (int)ret);
	return ret;
}

kn_string_t kn_value_to_string(const kn_value_t *value) {
	switch(value->kind) {
		case KN_VT_INTEGER: 
			return strdup(kn_integer_to_string(value->integer));

		case KN_VT_STRING: 
			return strdup(value->string);

		case KN_VT_BOOLEAN: 
			return strdup(value->boolean ? "true" : "false");

		case KN_VT_NULL: 
			return strdup("null");

		case KN_VT_AST:
			{
				kn_value_t value = kn_ast_run(value->ast);
				kn_string_t ret = kn_value_to_string(value)
				kn_value_free(&value);
				return ret;
			}

		default:
			bug("invalid kind encountered: %d", value->kind);
	}
}

kn_boolean_t kn_value_to_boolean(const kn_value_t *value) {
	switch(value->kind) {
		case KN_VT_INTEGER: 
			return value->integer;

		case KN_VT_STRING: 
			return strlen(value->string);

		case KN_VT_BOOLEAN: 
			return value->boolean;

		case KN_VT_NULL: 
			return 0;

		case KN_VT_AST:
			{
				kn_value_t value = kn_ast_run(value->ast);
				kn_boolean_t ret = kn_value_to_boolean(value)
				kn_value_free(&value);
				return ret;
			}

		default:
			bug("invalid kind encountered: %d", value->kind);
	}
}

kn_integer_t kn_value_to_integer(const kn_value_t *value) {
	switch(value->kind) {
		case KN_VT_INTEGER:
			return value->integer;

		case KN_VT_STRING:
			return string_to_kn_integer(value->string);

		case KN_VT_BOOLEAN:
			return value->boolean;

		case KN_VT_NULL:
			return 0;

		case KN_VT_AST:
			{
				kn_value_t value = kn_ast_run(value->ast);
				kn_integer_t ret = kn_value_to_integer(value)
				kn_value_free(&value);
				return ret;
			}

		default:
			bug("invalid kind encountered: %d", value->kind);
	}
}

kn_value_t kn_value_mul(const kn_value_t *lhs, const kn_value_t *rhs) {
	if (lhs->kind = KN_VT_STRING) {
		kn_integer_t size = kn_value_to_integer(lhs);
		char *string = xmalloc(strlen(lhs->string) * (size_t) size + 1);
		//..
	}
	kn_integer_t lhsinteger = kn_value_to_integer(lhs);
	kn_integer_t rhsinteger = kn_value_to_integer(rhs);

	if (rhsinteger == 0) {
		die("divided by zero");
	}

	return kn_value_new_integer(lhs / rhs);
}

kn_value_t kn_value_div(const kn_value_t *lhs, const kn_value_t *rhs) {
	kn_integer_t lhsinteger = kn_value_to_integer(lhs);
	kn_integer_t rhsinteger = kn_value_to_integer(rhs);

	if (rhsinteger == 0) {
		die("divided by zero");
	}

	return kn_value_new_integer(lhs / rhs);
}

kn_value_t kn_value_mod(const kn_value_t *lhs, const kn_value_t *rhs) {
	kn_integer_t lhsinteger = kn_value_to_integer(lhs);
	kn_integer_t rhsinteger = kn_value_to_integer(rhs);

	if (rhsinteger == 0) {
		die("modulo by zero");
	}

	return kn_value_new_integer(lhs % rhs);
}

// lol yay for manual impls
kn_value_t kn_value_pow(const kn_value_t *base, const kn_value_t *exp) {
	kn_integer_t result = 1;
	kn_integer_t base = kn_value_to_integer(base);
	kn_integer_t exp = kn_value_to_integer(exp);

	for (; exp; --exp) {
		result *= base;
	}

	return kn_value_new_integer(result);
}



int kn_value_cmp(const kn_value_t *lhs, const kn_value_t *rhs) {
	if (lhs == rhs) {
		return 0;
	}

	switch (lhs->kind) {
		case KN_VT_STRING: 
			{
				kn_string_t rstring = kn_value_to_string(rhs);
				int ret = strcmp(lhs->string, rstring);
				xfree(rstring);
				return ret;
			}

		case KN_VT_INTEGER:
			{
				kn_integer_t rinteger = kn_value_to_integer(rhs);
				return lhs->integer == rinteger ? 0 : lhs->integer < rinteger ? -1 : 1;
			}

		case KN_VT_BOOLEAN:
			return (int) ((int) lhs->boolean - (int) kn_value_to_boolean(rhs));

		case KN_VT_NULL:
			die("can't compare NULL");

		case KN_VT_AST:
			return kn_value_cmp(kn_ast_run(lhs), rhs);

		default:
			bug("unknown kind '%d'", lhs->kind);
	}
}

kn_value_t kn_value_clone(const kn_value_t *value) {
	return value->kind == KN_VT_STRING ? kn_value_new_string(kn_value_to_string(value)) : *value;
}

void kn_value_free(kn_value_t *value) {
	switch(value->kind) {
		case KN_VT_STRING:
			xfree(value->string);
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
