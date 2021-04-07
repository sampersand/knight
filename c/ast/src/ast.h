#ifndef KN_AST_H
#define KN_AST_H

#include "function.h" /* kn_function */
#include "value.h"    /* kn_value */

/*
 * The type that represents a function and its arguments in Knight.
 *
 * Note that this struct should be passed to `kn_ast_free` to release its
 * resources.
 */
 struct kn_ast {
	/*
	 * The function associated with this ast.
	 */
	_Alignas(16) const struct kn_function *func;

	/*
	 * How many references to this object exist.
	 */
	unsigned refcount;

	/*
	 * The arguments of this ast.
	 */
	kn_value args[];
};

/*
 * Allocates a new `kn_ast` with the given number of arguments.
 */
struct kn_ast *kn_ast_alloc(unsigned argc);

/*
 * Duplicates the ast, returning a new value that must be `kn_ast_free`d
 * independently from the passed `ast`.
 */
struct kn_ast *kn_ast_clone(struct kn_ast *ast);

/*
 * Releases the memory resources associated with this struct.
 */
void kn_ast_free(struct kn_ast *ast);

/*
 * Executes a `kn_ast`, returning the value associated with its execution.
 */
kn_value kn_ast_run(struct kn_ast *ast);

#endif /* !KN_AST_H */
