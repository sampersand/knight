#ifndef KN_AST_H
#define KN_AST_H

#include "env.h"
#include "value.h"


struct kn_ast_t;
typedef const char ** stream_t;

struct kn_ast_t *kn_ast_parse(stream_t);
struct kn_ast_t *kn_ast_parse_nonnull(stream_t);

struct kn_value_t kn_ast_run(const struct kn_ast_t *);
struct kn_ast_t *kn_ast_clone(const struct kn_ast_t *);
void kn_ast_free(struct kn_ast_t *);

void kn_ast_dump(const struct kn_ast_t *);

#endif /* KN_AST_H */
