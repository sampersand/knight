#include <stdlib.h>
#include <string.h>

#include "value.h"
#include "ast.h"
#include "shared.h"

struct kn_value_t kn_value_new_ast(struct kn_ast_t *ast) {
	return (struct kn_value_t) {.kind = KN_VT_AST, .ast = ast };
}

struct kn_value_t kn_value_new_string(struct kn_string_t string) {
	return (struct kn_value_t) {.kind = KN_VT_STRING, .string = string };
}

struct kn_value_t kn_value_new_integer(kn_integer_t integer) {
	return (struct kn_value_t) {.kind = KN_VT_INTEGER, .integer = integer };
}

struct kn_value_t kn_value_new_boolean(kn_boolean_t boolean) {
	return (struct kn_value_t) {.kind = KN_VT_BOOLEAN, .boolean = boolean };
}

struct kn_value_t kn_value_new_null(void) {
	return (struct kn_value_t) {.kind = KN_VT_NULL };
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
		break;

	case KN_VT_INTEGER:
	case KN_VT_BOOLEAN:
	case KN_VT_NULL:
		break;

	default:
		bug("unknown value kind '%d'", value->kind);
	}
}
