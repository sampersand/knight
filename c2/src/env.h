#ifndef KN_ENV_H
#define KN_ENV_H

#include "value.h"

void kn_env_init(size_t);
void kn_env_free(void);
kn_value_t *kn_env_fetch(const char *, _Bool);
const char *kn_env_name_for(kn_value_t *);

#endif
