#ifndef KN_ENV_H
#define KN_ENV_H

#include "value.h"


// initializes the env with the starting capacity.
void kn_env_initialize(size_t);

// get the value, return `NULL` if it doesnt exist
struct kn_value_t *kn_env_get(const char *);

// return a ref to the original one.
void kn_env_set(const char *, struct kn_value_t);

#endif /* KN_ENV_H */
