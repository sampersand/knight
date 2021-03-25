#ifndef KN_AST_H
#define KN_AST_H

#include "function.h" /* kn_function_t */
#include "value.h"    /* kn_value_t */
#include <stdbool.h>  /* bool */

#ifdef KN_DYNMAIC_ARGC
#define KN_AST_ARITY(ast) ((ast)->argc)
#else
#define KN_AST_ARITY(ast) ((ast)->func->arity)
#endif /* KN_DYNMAIC_ARGC */

/*
 * The type that represents a function and its arguments in Knight.
 *
 * Note that this struct should be passed to `kn_ast_free` to release its
 * resources.
 */
struct kn_ast_t {
	/*
	 * The function associated with this ast.
	 */
	const struct kn_function_t *func;

	/*
	 * How many references to this object exist.
	 */
	unsigned refcount;

#ifdef KN_DYNMAIC_ARGC
	/*
	 * One of the extensions allows us to have a dynamic argc (ie collapse
	 * adjacent calls to the same function).
	 */
	unsigned const argc;
#endif /* KN_DYNMAIC_ARGC */

	/*
	 * The arguments to this struct.
	 */
	kn_value_t args[];
};

/*
 * Allocates a new `kn_ast_t` with the given number of arguments.
 */
struct kn_ast_t *kn_ast_alloc(unsigned argc);

/*
 * Duplicates the ast, returning a new value that must be `kn_ast_free`d
 * independently from the passed `ast`.
 */
struct kn_ast_t *kn_ast_clone(struct kn_ast_t *ast);

/*
 * Releases the memory resources associated with this struct.
 */
void kn_ast_free(struct kn_ast_t *ast);

/*
 * Executes a `kn_ast_t`, returning the value associated with its execution.
 */
kn_value_t kn_ast_run(struct kn_ast_t *ast, bool owned);


#endif /* KN_AST_H */
