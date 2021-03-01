#ifndef KN_ENV_H
#define KN_ENV_H

#include "value.h"

kn_value_t kn_env_get(const char *);
void kn_env_set(struct kn_string_t *, kn_value_t);

#endif
