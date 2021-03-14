#ifndef KN_KNIGHT_H
#define KN_KNIGHT_H

#include "value.h"

void kn_init(size_t capacity);
void kn_free(void);
kn_value_t kn_run(const char *stream);

#endif
