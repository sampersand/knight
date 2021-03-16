#ifndef ENV_H
#define ENV_H

#include "value.h"

typedef struct _variable_t {
	value_t value;
	const char *name;
} variable_t;

void env_init(size_t);
void env_free(void);
variable_t *env_fetch(const char *, bool);

#endif
