#ifndef KN_ENV_H
#define KN_ENV_H

#include "value.h"  /* kn_value_t */

#include <stddef.h>  /* size_t */
#include <stdbool.h> /* bool */

/*
 * A variable within Knight.
 *
 * This struct is only returned via `kn_env_fetch`, and lives for the remainder
 * of the program's lifetime. (Or, at least until `kn_env_free` is called.) As
 * such, there is no need to free it.
 */
struct kn_variable_t;

/*
 * Initializes the global Knight environment with the given starting capacity.
 *
 * This _must_ be called before `kn_env_fetch` is called.
 */
void kn_env_startup(void);

/*
 * Frees all resources associated with the global Knight environment.
 *
 * This will invalidate all `kn_variable_t` pointers, and `kn_env_startup` must
 * be called again before `kn_env_fetch` can be used.
 */
void kn_env_shutdown(void);

/*
 * Fetches the variable associated with the given identifier.
 *
 * If ownership of `identifier` is being transferred to this function, `owned`
 * should be set to `true`; if simply a reference is being given, it should be
 * `false`.
 *
 * This will always return a `kn_variable_t`, which may have been newly created.
 */
struct kn_variable_t *kn_env_fetch(const char *identifier, bool owned);

/*
 * Assigns a value to this variable, overwriting whatever was there previously.
 */
void kn_variable_assign(struct kn_variable_t *variable, kn_value_t);

/*
 * Runs the given variable, returning the value associated with it.
 *
 * If the variable has not been assigned to yet, this will abort the program.
 */
kn_value_t kn_variable_run(struct kn_variable_t *variable);

/*
 * Fetches the name of the variable.
 */
const char *kn_variable_name(const struct kn_variable_t *variable);

#endif /* !KN_ENV_H */
