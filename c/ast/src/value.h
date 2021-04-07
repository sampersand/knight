#ifndef KN_VALUE_H
#define KN_VALUE_H

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
typedef uint64_t kn_value;

/*
 * The number type within Knight.
 *
 * Technically, this implementation only supports `int63_t` (as the extra bit
 * is used to indicate whether a `kn_value`'s a number or something else).
 */
typedef int64_t kn_number;

/*
 * The boolean type within Knight.
 *
 * This simply exists for completeness and functions identically to a `bool`.
 */
typedef bool kn_boolean;

// Forward declarations.
struct kn_ast;
struct kn_string;
struct kn_variable;

#define KN_SHIFT 3
#define KN_TAG_CONSTANT 0
#define KN_TAG_NUMBER 1
#define KN_TAG_VARIABLE 2
#define KN_TAG_STRING 3
#define KN_TAG_AST 4

#define KN_TAG_MASK ((1 << KN_SHIFT) - 1)
#define KN_TAG(x) ((x) & KN_TAG_MASK)
#define KN_UNMASK(x) ((x) & ~KN_TAG_MASK)

/*
 * The false value within Knight.
 */
#define KN_FALSE (0 | KN_TAG_CONSTANT)

/*
 * The null value within Knight.
 */
#define KN_NULL (8 | KN_TAG_CONSTANT)

/*
 * The true value within Knight.
 */
#define KN_TRUE (16 | KN_TAG_CONSTANT)

/*
 * An undefined value, used to indicate "no value."
 *
 * This is used in a few places, such as the default value for variables, and
 * what's returned from `kn_parse` if no values could e parsed. This value is
 * invalid to pass to any function expecting a valid `kn_value`.
 */
#define KN_UNDEFINED (24 | KN_TAG_CONSTANT)

/*
 * The alignment required for all values.
 */
#define KN_VALUE_ALIGN 8

/*
 * Creates a new number value.
 *
 * Note that `number` has to be a valid `kn_number`---see its typedef for more
 * details on what this entails.
 */
kn_value kn_value_new_number(kn_number number);

/*
 * Creates a new boolean value.
 *
 * If you know the value of `boolean` ahead of time, you should simply use
 * `KN_TRUE` or `KN_FALSE`.
 */
kn_value kn_value_new_boolean(kn_boolean boolean);

/*
 * Creates a new string value.
 *
 * This passes ownership of the string to this function, and any use of the
 * passed pointer is invalid after this function returns.
 */
kn_value kn_value_new_string(struct kn_string *string);

/*
 * Creates a new variable value.
 */
kn_value kn_value_new_variable(struct kn_variable *variable);

/*
 * Creates a new ast value.
 *
 * This passes ownership of the ast to this function, and any use of the
 * passed pointer is invalid after this function returns.
 */
kn_value kn_value_new_ast(struct kn_ast *ast);

/*
 * Checks to see if `value` is a `kn_number`.
 */
#define KN_VALUE_IS_NUMBER(value) (((value) & KN_TAG_NUMBER) == KN_TAG_NUMBER)

/*
 * Checks to see if `value` is a `KN_TRUE` or `KN_FALSE`.
 */
#define KN_VALUE_IS_BOOLEAN(value) ((value) == KN_FALSE || (value) == KN_TRUE)

/*
 * Note there's no `kn_value_is_null`, as you can simply do `value == KN_NULL`.
 */

/*
 * Checks to see if `value` is a `kn_string`.
 */
#define KN_VALUE_IS_STRING(value) (((value) & KN_TAG_STRING) == KN_TAG_STRING)

/*
 * Checks to see if `value` is a `kn_string`.
 */
#define KN_VALUE_IS_VARIABLE(value) (((value) & KN_TAG_VARIABLE) == KN_TAG_VARIABLE)

/*
 * Checks to see if `value` is a `kn_ast`.
 */
#define KN_VALUE_IS_AST(value) (((value) & KN_TAG_AST) == KN_TAG_AST)


/*
 * Retrieves the `kn_number` associated with `value`.
 *
 * This should only be called on number values.
 */
kn_number kn_value_as_number(kn_value value);

/*
 * Retrieves the `kn_boolean` associated with `value`.
 *
 * This should only be called on boolean values.
 */
kn_boolean kn_value_as_boolean(kn_value value);

/*
 * Retrieves the `kn_string` associated with `value`.
 *
 * This should only be called on string values.
 */
struct kn_string *kn_value_as_string(kn_value value);

/*
 * Retrieves the `kn_variable` associated with `value`.
 *
 * This should only be called on variable values.
 */
struct kn_variable *kn_value_as_variable(kn_value value);

/*
 * Retrieves the `kn_ast` associated with `value`.
 *
 * This should only be called on ast values.
 */
struct kn_ast *kn_value_as_ast(kn_value value);

/*
 * Converts the `value` to a `kn_number`, coercing it if need be.
 */
kn_number kn_value_to_number(kn_value value);

/*
 * Converts the `value` to a `kn_boolean`, coercing it if need be.
 */
kn_boolean kn_value_to_boolean(kn_value value);

/*
 * Converts the `value` to a `kn_string`, coercing it if need be.
 *
 * Note that the caller must free the returned string via `kn_string_free`.
 */
struct kn_string *kn_value_to_string(kn_value value);

/*
 * Dumps the debugging representation of `value` to stdout, without a trailing
 * newline.
 */
void kn_value_dump(kn_value value);

/*
 * Executes the given value.
 *
 * The returned value must be passed to `kn_value_free` to prevent memory leaks.
 */
kn_value kn_value_run(kn_value value);

/*
 * Returns a copy of `value`.
 *
 * Both `value` and the returned value must be passed independently to
 * `kn_value_free` to ensure that all resources are cleaned up after use.
 */
kn_value kn_value_clone(kn_value value);

/*
 * Frees all resources associated with `value`.
 *
 * Note that for the refcounted types (ie `kn_string` and `kn_ast`), this
 * will only actually free the resources when the refcount hits zero.
 */
void kn_value_free(kn_value value);

#endif /* !KN_VALUE_H */
