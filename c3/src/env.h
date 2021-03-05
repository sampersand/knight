#ifndef KN_ENV_H
#define KN_ENV_H

#include "value.h"

typedef struct _kn_variable_t {
	kn_value_t value;
	const char *name;
} kn_variable_t;

void kn_env_init(size_t);
void kn_env_free(void);
kn_variable_t *kn_env_fetch(const char *, bool);

#endif
