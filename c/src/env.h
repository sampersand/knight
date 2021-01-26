#ifndef KN_ENV_H
#define KN_ENV_H

#include "value.h" /* kn_value_t */

/*
 * Initialize the environment with the starting capacity.
 *
 * `capacity` must be less than or equal to `SIZE_MAX / sizeof(kn_value_t)`,
 * and may not be zero.
 *
 * This must be run before any Knight code is executed.
 */
void kn_env_init(size_t capacity);

/*
 * Frees all memory associated with the environment
 */
void kn_env_free(void);

/*
 * Gets the value associated with the given identifier.
 *
 * If the identifier is unknown, `NULL` is returned.
 *
 * The identifier must not be `NULL`. Additionally, the caller must not call
 * `kn_value_free` on the returned value.
 */
const struct kn_value_t *kn_env_get(const char *identifier);

/*
 * Assigns an identifier to a value.
 *
 * The identifier must not be `NULL`. The value passed to this function
 * must not be freed by anyone else---it's "ownership" is passed to the
 * environment.
 */
void kn_env_set(const char *identifier, struct kn_value_t value);

#endif /* KN_ENV_H */
