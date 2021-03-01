#ifndef VALUE_H
#define VALUE_H

#include <stdint.h>
#include "string.h"
#include <stdbool.h>

typedef uint64_t kn_value_t;
typedef int64_t kn_number_t;
typedef bool kn_boolean_t;

struct kn_ast_t;

#define KN_FALSE 0
#define KN_NULL 2
#define KN_TRUE 4


kn_value_t kn_value_new_number(kn_number_t);
kn_value_t kn_value_new_boolean(kn_boolean_t);
kn_value_t kn_value_new_string(const struct kn_string_t *);
kn_value_t kn_value_new_identifier(const struct kn_string_t *);
kn_value_t kn_value_new_ast(const struct kn_ast_t *);

kn_number_t kn_value_to_number(kn_value_t);
kn_boolean_t kn_value_to_boolean(kn_value_t);
const struct kn_string_t *kn_value_to_string(kn_value_t);

void kn_value_dump(kn_value_t);

kn_value_t kn_value_run(kn_value_t);
void kn_value_free(kn_value_t);
kn_value_t kn_value_clone(kn_value_t);

#endif
