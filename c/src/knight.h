#ifndef KN_H
#define KN_H

#include "ast.h"
#include "value.h"

void kn_init(void);
struct kn_value_t kn_run(const char *stream);

#endif /* KN_H */
