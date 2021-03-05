#ifndef KN_FUNCTION_H
#define KN_FUNCTION_H

#include "ast.h"   /* kn_ast_t */
#include "value.h" /* kn_value_t */

/*
 * The type that represents a function within knight.
 */
struct kn_function_t {
	size_t arity;
	struct kn_value_t (*func)(const struct kn_ast_t *args);
	char name; // for debugging purposes.
};

/*
 * Gets the function associated with `name`, returning `NULL` if no function
 * is registered.
 *
 * The returned pointer is valid for the lifetime of the program. However, it
 * will always point to the current function associated with `name`. This can
 * be overwritten via `kn_fn_register_func`.
 */
const struct kn_function_t *kn_fn_fetch(char name);

/*
 * Registers a custom function with Knight, that is accessible within Knight
 * source code.
 *
 * The `name` supplied must be a valid, unique character. See `ast.c` for a list
 * of function names that are reserved by Knight. Execution will be aborted if
 * an additional name is attempted to be added.
 */
void kn_fn_register_func(struct kn_function_t func);


/*************************************************
 *                  Arity  Zero                  *
 *************************************************/


/*
 * `TRUE`: The "TRUE" function; this just returns a `true` `kn_value_t`.
 */
struct kn_value_t kn_fn_true(const struct kn_ast_t *ast);

/*
 * `FALSE`: The "FALSE" function; this just returns a `false` `kn_value_t`.
 */
struct kn_value_t kn_fn_false(const struct kn_ast_t *ast);

/*
 * `NULL`: The "NULL" function; this just returns a `null` `kn_value_t`.
 */
struct kn_value_t kn_fn_null(const struct kn_ast_t *ast);

/*
 * `PROMPT`: Reads a single line from stdin and returns it.
 *
 * If stdin is at EOF, an empty string is returned.
 */
struct kn_value_t kn_fn_prompt(const struct kn_ast_t *ast);

/*
 * `RAND`: Returns a random `kn_integer_t`.
 */
struct kn_value_t kn_fn_rand(const struct kn_ast_t *ast);


/************************************************
 *                   Arity One                  *
 ************************************************/


/*
 * `EVAL str`: Evaluates `str` as if it were Knight code.
 *
 * If an only-whitespace/comments string is given, `null` is returned.
 */
struct kn_value_t kn_fn_eval(const struct kn_ast_t *ast);

/*
 * `BLOCK arg`: Returns `arg`, unevaluated.
 *
 * This is used to delay execution; blocks can be executed via the
 * function `KN_TT_CALL`.
 */
struct kn_value_t kn_fn_block(const struct kn_ast_t *ast);

/*
 * `CALL block`: Executes a block of code that was created via
 * `KN_TT_BLOCK`, returning that expression's result.
 *
 * If given a non-block value, it aborts execution.
 */
struct kn_value_t kn_fn_call(const struct kn_ast_t *ast);

/*
 * `` ` str ``: Executes `str` as a system command, returning the
 * stdout of that function.
 *
 * If there are problems during the opening of the command, the program is
 * aborted. (ie the Knight program.)
 */
struct kn_value_t kn_fn_system(const struct kn_ast_t *ast);

/*
 * `QUIT code`: Halts execution with the status code `code`.
 *
 * The first argument, after being converted to a number, must be able to be fit
 * within an `int`.
 */
struct kn_value_t kn_fn_quit(const struct kn_ast_t *ast);

/*
 * `! arg`: Converts `arg` to a boolean, then returns the logical
 * negation of it.
 */
struct kn_value_t kn_fn_not(const struct kn_ast_t *ast);

/*
 * `LENGTH str`: Returns the length of `str`.
 */
struct kn_value_t kn_fn_length(const struct kn_ast_t *ast);

/*
 * `DUMP value`: Runs `value`, writing its result to stdout. Returns the result
 * of running `value`.
 */
struct kn_value_t kn_fn_run(const struct kn_ast_t *ast);

/*
 * `OUTPUT msg`: Outputs `msg` as a string, followed by a newline.
 *
 * If `msg` ends with a backslash (`\`), then the newline is suppressed.
 */
struct kn_value_t kn_fn_output(const struct kn_ast_t *ast);


/*************************************************
 *                   Arity Two                   *
 *************************************************/


/*
 * `+ lhs rhs`: If `lhs` is a string, `rhs` is converted to a string
 * and concatenated with `lhs`. Otherwise, both are converted to numbers
 * and added together.
 */
struct kn_value_t kn_fn_add(const struct kn_ast_t *ast);

/*
 * `- lhs rhs`: Converts both arguments to integers and subtracts
 * `rhs` from `lhs`.
 */
struct kn_value_t kn_fn_sub(const struct kn_ast_t *ast);

/*
 * `* lhs rhs`: If `lhs` is a string, then `lhs` concatenated with
 * itself `rhs` times is returned. Otherwise, both are converted to an
 * integer and multiplied together.
 */
struct kn_value_t kn_fn_mul(const struct kn_ast_t *ast);

/*
 * `/ lhs rhs`: Converts both arguments to integers and divides `lhs`
 * by `rhs`.
 *
 * Execution is aborted if the second argument is zero.
 */
struct kn_value_t kn_fn_div(const struct kn_ast_t *ast);

/*
 * `% lhs rhs`: Converts both arguments to integers and modulos `lhs`
 * by `rhs`.
 *
 * Execution is aborted if the second argument is zero.
 */
struct kn_value_t kn_fn_mod(const struct kn_ast_t *ast);

/*
 * `^ lhs rhs`: Raises `lhs` to the power of `rhs`. Returns `1` if
 * `rhs` is zero.
 */
struct kn_value_t kn_fn_pow(const struct kn_ast_t *ast);

/*
 * `? lhs rhs`: Returns `true` only if both values are the same kind, and
 * are the same value for that type.
 *
 * NOTE: This is the only function that doesn't do automatic conversion
 * of arguments. To do conversions, you can use `& < l r > l r`.
 */
struct kn_value_t kn_fn_eql(const struct kn_ast_t *ast);

/*
 * `< lhs rhs`: If `lhs` is a string, checks to see if `lhs` is less
 * than `rhs` lexicographically. Othewise, both are converted to numbers 
 * and then compared.
 */
struct kn_value_t kn_fn_lth(const struct kn_ast_t *ast);

/*
 * `> lhs rhs`: If `lhs` is a string, checks to see if `lhs` is less
 * than `rhs` lexicographically. Othewise, both are converted to numbers 
 * and then compared.
 */
struct kn_value_t kn_fn_gth(const struct kn_ast_t *ast);

/*
 * `& lhs rhs`: Executes `lhs`. If that's falsey, it's returned 
 * directly; otherwise, `rhs` is executed and its result is returned.
 */
struct kn_value_t kn_fn_and(const struct kn_ast_t *ast);

/*
 * `| lhs rhs`: Executes `lhs`. If that's truthy, it's returned 
 * directly; otherwise, `rhs` is executed and its result is returned.
 */
struct kn_value_t kn_fn_or(const struct kn_ast_t *ast);

/*
 * `; lhs rhs`: Executes `lhs`, then `rhs` is executed, returning
 * `rhs`'s result.
 */
struct kn_value_t kn_fn_then(const struct kn_ast_t *ast);

/*
 * `WHILE cond body`: Executes the `body` continuously until `cond`
 * becomes falsey.
 *
 * Returns the last result of the body, or `null` if the body wasn't
 * executed at all.
 */
struct kn_value_t kn_fn_while(const struct kn_ast_t *ast);

/*
 * `= ident arg`: Assigns the global variable `ident` to `arg`.
 *
 * Execution is aborted if the first argument isn't an identifier.
 */
struct kn_value_t kn_fn_assign(const struct kn_ast_t *ast);


/*************************************************
 *                  Arity Three                  *
 *************************************************/


/*
 * `I cond if_t if_f`: If `cond` is truthy, `if_t` is executed and
 * returned. Otherwise, `if_f` is executed and returned.
 */
struct kn_value_t kn_fn_if(const struct kn_ast_t *ast);

/*
 * `GET str idx len`: Gets a substring from `str`, starting at `idx` and 
 * is at most `len` characters long. (It can be less if `str + idx` is
 * past the end of the array)
 *
 * If `idx` is greater than `str`'s length, an empty string is returned.
 */
struct kn_value_t kn_fn_get(const struct kn_ast_t *ast);


/*************************************************
 *                  Arity  Four                  *
 *************************************************/


/*
 * `SET str idx len repl`: Returns a copy of `str`, with the substring
 * starting at `idx`, and at most `len` characters long, replaced with
 * `repl`.
 *
 * If `idx` is greater than `str`'s length, the original string is
 * returned.
 */
struct kn_value_t kn_fn_set(const struct kn_ast_t *ast);

#endif /* KN_FUNCTION_H */
