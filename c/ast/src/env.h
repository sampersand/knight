#ifndef KN_ENV_H
#define KN_ENV_H

#include "value.h"   /* kn_value */
#include <stdbool.h> /* bool */

/*
 * A variable within Knight.
 *
 * This struct is only returned via `kn_env_fetch`, and lives for the remainder
 * of the program's lifetime. (Or, at least until `kn_env_free` is called.) As
 * such, there is no need to free it.
 */
struct kn_variable {
	/*
	 * The value associated with this variable.
	 *
	 * When a variable is first fetched, this is set to `KN_UNDEFINED`, and
	 * should be overwritten before being used.
	 */
	kn_value value;

	/*
	 * The name of this variable.
	 */
	const char *name;
};

/*
 * Initializes the global Knight environment.
 *
 * This _must_ be called before `kn_env_fetch` is called.
 */
void kn_env_startup(void);

/*
 * Frees all resources associated with the global Knight environment.
 *
 * This will invalidate all `kn_variable` pointers, and `kn_env_startup` must
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
 * This will always return a `kn_variable`, which may have been newly created.
 */
struct kn_variable *kn_env_fetch(const char *identifier, bool owned);

/*
 * Assigns a value to this variable, overwriting whatever was there previously.
 */
void kn_variable_assign(struct kn_variable *variable, kn_value);

/*
 * Runs the given variable, returning the value associated with it.
 *
 * If the variable has not been assigned to yet, this will abort the program.
 */
kn_value kn_variable_run(struct kn_variable *variable);

#endif /* !KN_ENV_H */
