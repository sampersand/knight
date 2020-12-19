#ifndef ENV_H
#define ENV_H

#include "value.h"

// get the value, return `NULL` if it doesnt exist
const kn_value_t *kn_env_get(const char *);

// return a ref to the original one.
const kn_value_t *kn_env_set(const char *, kn_value_t);

#endif /* ENV_H */
