#ifndef VALUE_H
#define VALUE_H

#include <stdint.h>
#include "string.h"
#include <stdbool.h>

typedef uint64_t kn_value_t;
typedef int64_t kn_number_t;
typedef bool kn_boolean_t;

struct kn_function_t;
struct kn_variable_t;

/*
 * The that represents a function and its arguments within Knight.
 *
 * Note that this struct itself is
 */
struct kn_ast_t {
	struct kn_function_t *func;
	unsigned refcount;

#ifdef KN_DYNMAIC_ARGC
	unsigned argc;
#endif

	kn_value_t args[];
};

#define KN_FALSE 0
#define KN_NULL 2
#define KN_TRUE 4
#define KN_UNDEFINED 8

kn_value_t kn_value_new_number(kn_number_t number);
kn_value_t kn_value_new_boolean(kn_boolean_t boolean);
kn_value_t kn_value_new_string(struct kn_string_t *string);
kn_value_t kn_value_new_variable(struct kn_variable_t *variable);
kn_value_t kn_value_new_ast(struct kn_ast_t *ast);

bool kn_value_is_number(kn_value_t value);
bool kn_value_is_boolean(kn_value_t value);
bool kn_value_is_string(kn_value_t value);
bool kn_value_is_null(kn_value_t value);
bool kn_value_is_variable(kn_value_t value);
bool kn_value_is_ast(kn_value_t value);

kn_number_t kn_value_as_number(kn_value_t value);
kn_boolean_t kn_value_as_boolean(kn_value_t value);
struct kn_string_t *kn_value_as_string(kn_value_t value);
struct kn_variable_t *kn_value_as_variable(kn_value_t value);
struct kn_ast_t *kn_value_as_ast(kn_value_t value);

kn_number_t kn_value_to_number(kn_value_t value);
kn_boolean_t kn_value_to_boolean(kn_value_t value);
struct kn_string_t *kn_value_to_string(kn_value_t value);

void kn_value_dump(kn_value_t value);

kn_value_t kn_value_run(kn_value_t value);
void kn_value_free(kn_value_t value);
kn_value_t kn_value_clone(kn_value_t value);

#endif
