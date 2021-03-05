#ifndef ENV_H
#define ENV_H

#include "value.h"

typedef struct _kn_variable_t {
	kn_value_t value;
	char *name;
} kn_variable_t;

#endif
