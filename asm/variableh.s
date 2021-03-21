/*
 * A variable within Knight.
 *
 * This struct is only returned via `kn_env_fetch`, and lives for the remainder
 * of the program's lifetime. (Or, at least until `kn_env_free` is called.)
 * As such, there is no need to free it.
 */
struct kn_variable_t {
	/*
	 * The value associated with this variable.
	 *
	 * When a variable is first fetched, this is set to `KN_UNDEFINED`, and
	 * should be overwritten before being used.
	 */
	kn_value_t value;

	/*
	 * The name of this variable.
	 */
	const char *name;
};
