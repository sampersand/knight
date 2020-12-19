#ifndef KN_VALUE_H
#define KN_VALUE_H
#include <stddef.h>
#include <stdbool.h>

typedef char * kn_string_t;
typedef __int128_t kn_integer_t;
typedef bool kn_boolean_t;

struct kn_ast_t;

typedef enum {
	KN_VT_STRING,
	KN_VT_BOOLEAN,
	KN_VT_INTEGER,
	KN_VT_NULL,
	KN_VT_AST,
} kn_value_kind_t;

typedef struct {
	kn_value_kind_t kind;

	union {
		kn_boolean_t boolean;
		kn_integer_t integer;
		kn_string_t string;
	};

} kn_value_t;

kn_value_t kn_value_new_ast(kn_ast_t *);
kn_value_t kn_value_new_string(kn_string_t);
kn_value_t kn_value_new_integer(kn_integer_t);
kn_value_t kn_value_new_boolean(kn_boolean_t);
kn_value_t kn_value_new_null(void);

kn_string_t  kn_value_to_string(const kn_value_t *);
kn_boolean_t kn_value_to_boolean(const kn_value_t *);
kn_integer_t kn_value_to_integer(const kn_value_t *);

kn_value_t kn_value_clone(const kn_value_t *);
void kn_value_free(kn_value_t *);

#endif /* KN_VALUE_H */
