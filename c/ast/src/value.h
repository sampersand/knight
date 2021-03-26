#ifndef KN_VALUE_H
#define KN_VALUE_H

#include "string.h"  /* kn_string_t */
#include <stdint.h>  /* uint64_t, int64_t */
#include <stdbool.h> /* bool */

/*
 * The type that represents values within Knight.
 *
 * All the different types within knight are represented in this type (which
 * internally uses bit masks). It's intentionally an opaque object---to interact
 * with it, you should use the relevant functions.
 *
 * To duplicate a value, use the `kn_value_clone` function---this returns a new
 * value which must be freed separately from the given one. To free a value,
 * pass it to `kn_value_free`.
 */
typedef uint64_t kn_value_t;

/*
 * The number type within Knight.
 *
 * Technically, this implementation only supports `int63_t` (as the extra bit
 * is used to indicate whether a `kn_value_t`'s a number or something else).
 */
typedef int64_t kn_number_t;

/*
 * The boolean type within Knight.
 *
 * This simply exists for completeness and functions identically to a `bool`.
 */
typedef bool kn_boolean_t;

// Forward declarations.
struct kn_function_t;
struct kn_variable_t;
struct kn_ast_t;

#ifdef KN_EXT_CUSTOM_TYPES
struct kn_custom_t;
#endif /* KN_EXT_CUSTOM_TYPES */

/*
 * The false value within Knight.
 */
#define KN_FALSE 0

/*
 * The null value within Knight.
 */
#define KN_NULL 2

/*
 * The true value within Knight.
 */
#define KN_TRUE 4

/*
 * An undefined value, used to indicate "no value."
 *
 * This is used in a few places, such as the default value for variables, and
 * what's returned from `kn_parse` if no values could e parsed. This value is
 * invalid to pass to any function expecting a valid `kn_value_t`.
 */
#define KN_UNDEFINED 8

/*
 * Creates a new number value.
 *
 * Note that `number` has to be a valid `kn_number_t`---see its typedef for more
 * details on what this entails.
 */
kn_value_t kn_value_new_number(kn_number_t number);

/*
 * Creates a new boolean value.
 *
 * If you know the value of `boolean` ahead of time, you should simply use
 * `KN_TRUE` or `KN_FALSE`. 
 */
kn_value_t kn_value_new_boolean(kn_boolean_t boolean);

/*
 * Creates a new string value.
 *
 * This passes ownership of the string to this function, and any use of the
 * passed pointer is invalid after this function returns.
 */
kn_value_t kn_value_new_string(struct kn_string_t *string);

/*
 * Creates a new variable value.
 */
kn_value_t kn_value_new_variable(struct kn_variable_t *variable);

/*
 * Creates a new ast value.
 *
 * This passes ownership of the ast to this function, and any use of the
 * passed pointer is invalid after this function returns.
 */
kn_value_t kn_value_new_ast(struct kn_ast_t *ast);

#ifdef KN_EXT_CUSTOM_TYPES
/*
 * Creates a new custom value.
 *
 * Ownership of the `custom` is passed to this function.
 */
kn_value_t kn_value_new_custom(struct kn_custom_t *custom);
#endif /* KN_EXT_CUSTOM_TYPES */

/*
 * Checks to see if `value` is a `kn_number_t`.
 */
bool kn_value_is_number(kn_value_t value);

/*
 * Checks to see if `value` is a `KN_TRUE` or `KN_FALSE`.
 */
bool kn_value_is_boolean(kn_value_t value);

/* Note there's no `kn_value_is_null`, as you can simply do `== KN_NULL`. */

/*
 * Checks to see if `value` is a `kn_string_t`.
 */
bool kn_value_is_string(kn_value_t value);

/*
 * Checks to see if `value` is a `kn_string_t`.
 */
bool kn_value_is_variable(kn_value_t value);

/*
 * Checks to see if `value` is a `kn_ast_t`.
 */
bool kn_value_is_ast(kn_value_t value);

#ifdef KN_EXT_CUSTOM_TYPES
/*
 * Checks to see if `value` is a `kn_custom_t`.
 */
bool kn_value_is_custom(kn_value_t value);
#endif /* KN_EXT_CUSTOM_TYPES */

/*
 * Retrieves the `kn_number_t` associated with `value`.
 *
 * This should only be called on number values.
 */
kn_number_t kn_value_as_number(kn_value_t value);

/*
 * Retrieves the `kn_boolean_t` associated with `value`.
 *
 * This should only be called on boolean values.
 */
kn_boolean_t kn_value_as_boolean(kn_value_t value);

/*
 * Retrieves the `kn_string_t` associated with `value`.
 *
 * This should only be called on string values.
 */
struct kn_string_t *kn_value_as_string(kn_value_t value);

/*
 * Retrieves the `kn_variable_t` associated with `value`.
 *
 * This should only be called on variable values.
 */
struct kn_variable_t *kn_value_as_variable(kn_value_t value);

/*
 * Retrieves the `kn_ast_t` associated with `value`.
 *
 * This should only be called on ast values.
 */
struct kn_ast_t *kn_value_as_ast(kn_value_t value);

#ifdef KN_EXT_CUSTOM_TYPES
/*
 * Retrieves the `kn_custom_t` associated with `value`.
 *
 * This should only be called on custom values.
 */
struct kn_custom_t *kn_value_as_custom(kn_value_t value);
#endif /* KN_EXT_CUSTOM_TYPES */

/*
 * Converts the `value` to a `kn_number_t`, coercing it if need be.
 */
kn_number_t kn_value_to_number(kn_value_t value);

/*
 * Converts the `value` to a `kn_boolean_t`, coercing it if need be.
 */
kn_boolean_t kn_value_to_boolean(kn_value_t value);

/*
 * Converts the `value` to a `kn_string_t`, coercing it if need be.
 *
 * Note that the caller must free the returned string via `kn_string_free`.
 */
struct kn_string_t *kn_value_to_string(kn_value_t value);

/*
 * Dumps the debugging representation of `value` to stdout.
 */
void kn_value_dump(kn_value_t value);

/*
 * Executes the given value.
 *
 * The returned value must be passed to `kn_value_free` to prevent memory leaks.
 */
kn_value_t kn_value_run(kn_value_t value);

/*
 * Returns a copy of `value`.
 *
 * Both `value` and the returned value must be passed independently to
 * `kn_value_free` to ensure that all resources are cleaned up after use.
 */
kn_value_t kn_value_clone(kn_value_t value);

/*
 * Frees all resources associated with `value`.
 *
 * Note that for the refcounted types (ie `kn_string_t` and `kn_ast_t`), this
 * will only actually free the resources when the refcount hits zero.
 */
void kn_value_free(kn_value_t value);

#endif /* !KN_VALUE_H */
