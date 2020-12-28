#ifndef KN_VALUE_H
#define KN_VALUE_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "string.h"
#include "integer.h"


typedef bool kn_boolean_t;
struct kn_ast_t;

enum kn_value_kind_t {
	KN_VT_STRING,
	KN_VT_BOOLEAN,
	KN_VT_INTEGER,
	KN_VT_NULL,
	KN_VT_AST,
};

struct kn_value_t {
	enum kn_value_kind_t kind;

	union {
		kn_boolean_t boolean;
		kn_integer_t integer;
		struct kn_string_t string;
		struct kn_ast_t *ast;
	};
};

struct kn_value_t kn_value_new_ast(struct kn_ast_t *);
struct kn_value_t kn_value_new_string(struct kn_string_t);
struct kn_value_t kn_value_new_integer(kn_integer_t);
struct kn_value_t kn_value_new_boolean(kn_boolean_t);
struct kn_value_t kn_value_new_null(void);

struct kn_string_t kn_value_to_string(const struct kn_value_t *);
kn_boolean_t kn_value_to_boolean(const struct kn_value_t *);
kn_integer_t kn_value_to_integer(const struct kn_value_t *);

// struct kn_value_t kn_value_add(const struct kn_value_t *, const struct kn_value_t *);
// struct kn_value_t kn_value_sub(const struct kn_value_t *, const struct kn_value_t *);
// struct kn_value_t kn_value_mul(const struct kn_value_t *, const struct kn_value_t *);
// struct kn_value_t kn_value_div(const struct kn_value_t *, const struct kn_value_t *);
// struct kn_value_t kn_value_mod(const struct kn_value_t *, const struct kn_value_t *);
// struct kn_value_t kn_value_pow(const struct kn_value_t *, const struct kn_value_t *);

// bool kn_value_eql(const struct kn_value_t *, const struct kn_value_t *);
// int kn_value_cmp(const struct kn_value_t *, const struct kn_value_t *);

struct kn_value_t kn_value_clone(const struct kn_value_t *);
void kn_value_free(struct kn_value_t *);

#endif /* KN_VALUE_H */
