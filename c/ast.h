#ifndef KN_AST_H
#define KN_AST_H

#include "env.h"
#include "value.h"

typedef struct kn_ast_t kn_ast_t;
typedef const char ** stream_t;

kn_ast_t *kn_ast_parse(stream_t);
kn_value_t kn_ast_run(const kn_ast_t *);
void kn_ast_free(kn_ast_t *);
void kn_ast_dump(const kn_ast_t *);

#endif /* KN_AST_H */
