#ifndef VALUE_H
#define VALUE_H

#include <stdint.h>
#include <stdbool.h>
#include "string.h"

struct _kn_variable_t;
struct _kn_block_t;

typedef uint64_t kn_value_t;
typedef int64_t kn_number_t;
typedef bool kn_boolean_t;

#define KN_FALSE 0
#define KN_NULL 2
#define KN_TRUE 4
#define KN_UNDEFINED 8

kn_value_t kn_value_new_number(kn_number_t);
kn_value_t kn_value_new_boolean(kn_boolean_t);
kn_value_t kn_value_new_string(kn_string_t *);
kn_value_t kn_value_new_variable(struct _kn_variable_t *);
kn_value_t kn_value_new_tag_block(struct _kn_block_t *);

bool kn_value_is_number(kn_value_t);
bool kn_value_is_boolean(kn_value_t);
bool kn_value_is_string(kn_value_t);
bool kn_value_is_variable(kn_value_t);
bool kn_value_is_tag_block(kn_value_t);

kn_number_t kn_value_as_number(kn_value_t);
kn_boolean_t kn_value_as_boolean(kn_value_t);
kn_string_t *kn_value_as_string(kn_value_t);
struct _kn_variable_t *kn_value_as_variable(kn_value_t);
struct _kn_block_t *kn_value_as_tag_block(kn_value_t);

kn_number_t kn_value_to_number(kn_value_t);
kn_boolean_t kn_value_to_boolean(kn_value_t);
kn_string_t *kn_value_to_string(kn_value_t);

void kn_value_dump(kn_value_t);

kn_value_t kn_value_clone(kn_value_t);
kn_value_t kn_value_run(kn_value_t);
void kn_value_free(kn_value_t);

#endif
