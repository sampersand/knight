#ifndef KN_AST_H
#define KN_AST_H

#include "env.h"
#include "value.h"

/*
 * The type that represents executable code within Knight.
 *
 * This type is created via `kn_ast_parse` and
 * should be disposed of via `kn_ast_free`.
 */
struct kn_ast_t;

// The "stream" type.
typedef const char ** stream_t;

/*
 * Parse a `kn_ast_t` from an input stream.
 *
 * Returns `NULL` if the stream is at an end, and `die`s if an error occurs
 * whilst reading the stream.
 */
struct kn_ast_t *kn_ast_parse(stream_t);

/*
 * Runs an `kn_ast_t`, returning the value associated with it.
 * 
 * If any errors occur whilst running the tree, the function will abort the
 * program with a message indicating the error.
 */
struct kn_value_t kn_ast_run(const struct kn_ast_t *);

/*
 * Clones an `kn_ast_t`.
 */
struct kn_ast_t *kn_ast_clone(const struct kn_ast_t *);

/*
 * Releases all the resources the `kn_ast_t` has associated with it.
 */
void kn_ast_free(struct kn_ast_t *);

// Debugging function used to inspect an AST.
void kn_ast_dump(const struct kn_ast_t *);

#endif /* KN_AST_H */
