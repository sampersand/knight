#ifndef VALUE_H
#define VALUE_H

#include <stdint.h>
#include <stdbool.h>
#include "string.h"

struct _variable_t;
struct _blockptr_t;

typedef uint64_t value_t;
typedef int64_t number_t;
typedef bool boolean_t;

#define FALSE_ 0
#define NULL_ 2
#define TRUE_ 4
#define UNDEFINED 8

value_t value_new_number(number_t);
value_t value_new_boolean(boolean_t);
value_t value_new_string(string_t *);
value_t value_new_variable(struct _variable_t *);
value_t value_new_block(struct _blockptr_t *);

bool value_is_number(value_t);
bool value_is_boolean(value_t);
bool value_is_string(value_t);
bool value_is_variable(value_t);
bool value_is_block(value_t);

number_t value_as_number(value_t);
boolean_t value_as_boolean(value_t);
string_t *value_as_string(value_t);
struct _variable_t *value_as_variable(value_t);
struct _blockptr_t *value_as_block(value_t);

number_t value_to_number(value_t);
boolean_t value_to_boolean(value_t);
string_t *value_to_string(value_t);

void value_dump(value_t);

value_t value_clone(value_t);
value_t value_run(value_t);
void value_free(value_t);

#endif
