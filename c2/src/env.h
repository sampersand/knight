#ifndef KN_ENV_H
#define KN_ENV_H

#include "value.h"
#include <stdbool.h>

struct kn_variable_t {
	kn_value_t value;
	const char *name;
};


void kn_env_init(size_t capacity);
void kn_env_free(void);
struct kn_variable_t *kn_env_fetch(const char *identifier, bool owned);

#endif
