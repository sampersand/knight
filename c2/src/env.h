#ifndef KN_ENV_H
#define KN_ENV_H

#include "value.h"

void kn_env_init(size_t);
void kn_env_free(void);
kn_value_t kn_env_get(const char *);
void kn_env_set(const char *, kn_value_t);

#endif
