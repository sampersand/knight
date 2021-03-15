#ifndef KN_KNIGHT_H
#define KN_KNIGHT_H

#include "value.h" /* kn_value_t */

/*
 * Begins the Knight interpreter.
 *
 * This function should be called before any other Knight function.
 *
 * Note that if the capacity is zero, the default capacity (defined by 
 * `KN_ENV_DEFAULT_CAPACITY`, which can be overwritten).
 */
void kn_init(size_t capacity);

/*
 * Frees all memory related to the current running Knight process.
 *
 * This invalidates _all_ pointers that Knight functions returned (including all
 * `kn_value_t`s).
 *
 * After this function is run, `kn_init` must be called again before calling any
 * other Knight functions.
 */
void kn_free(void);

/*
 * Executes the given stream as knight code.
 *
 * Note that any errors that may be caused during the execution of the code will
 * simply abort the program (like all exceptions in Knight do).
 */
kn_value_t kn_run(const char *stream);

#endif /* KN_KNIGHT_H */
