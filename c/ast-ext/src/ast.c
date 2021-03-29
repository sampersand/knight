#include "ast.h"    /* prototypes, kn_ast, kn_value_free, bool */
#include "shared.h" /* xmalloc */
#include <stdlib.h> /* free */

struct kn_ast *kn_ast_alloc(unsigned argc) {
	struct kn_ast *ast = xmalloc(
		sizeof(struct kn_ast) + sizeof(kn_value [argc]));

#ifdef KN_DYNMAIC_ARGC
	ast->argc = argc;
#endif /* KN_DYNMAIC_ARGC */

	return ast;
}

struct kn_ast *kn_ast_clone(struct kn_ast *ast) {
	++ast->refcount;

	return ast;
}

void kn_ast_free(struct kn_ast *ast) {
	if (--ast->refcount) // if we're not the last reference, leave early.
		return;

	for (unsigned i = 0; i < KN_AST_ARITY(ast); ++i)
		kn_value_free(ast->args[i]);

	free(ast);
}

kn_value kn_ast_run(struct kn_ast *ast) {
	return (ast->func->func)(ast->args);
}
