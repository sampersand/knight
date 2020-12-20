#ifndef KN_ENV_H
#define KN_ENV_H

#include "value.h"

// get the value, return `NULL` if it doesnt exist
kn_value_t *kn_env_get(const char *);

// return a ref to the original one.
void kn_env_set(const char *, kn_value_t);

#endif /* KN_ENV_H */
