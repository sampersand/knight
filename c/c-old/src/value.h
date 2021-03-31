#ifndef KN_VALUE_H
#define KN_VALUE_H

#include <stdint.h>  /* intmax_t */
#include "string.h"  /* kn_string_t */

/*
 * The type that's used to represent an integer in Knight.
 */
typedef intmax_t kn_integer_t;

/*
 * The type that's used to represent boolean values in Knight.
 */
typedef _Bool kn_boolean_t;

// forward declare the ast struct.
struct kn_ast_t;

/*
 * All the different types of values that can exist within Knight. Each variant
 * corresponds to a different field within `kn_value_t`:
 * - `KN_VT_STRING`: `string`
 * - `KN_VT_BOOLEAN`: `boolean`
 * - `KN_VT_INTEGER`: `integer`
 * - `KN_VT_NULL`: <none> (the tag identifies it as a null value).
 * - `KN_VT_AST`: `ast`
 */
enum kn_value_kind_t {
	KN_VT_STRING,
	KN_VT_BOOLEAN,
	KN_VT_INTEGER,
	KN_VT_NULL,
	KN_VT_AST,
};

/*
 * The type that represents all types within Knight.
 *
 * Each field corresponds to a `kn_value_kind_t` variant. The `ast` field is a
 * pointer as the `kn_ast_t` struct is not frequently assigned to a value (only
 * when `block` is used), and would bloat the size of the struct.
 *
 * NOTE: We could definitely optimize this with some pointer bit manipulation
 * tricks, but the C edition of Knight's intended to be as stable and readable
 * as possible; pointer manipulations would be neither.
 *
 * After a value is done being used, it must be passed to `kn_value_free` so as
 * to release any resources associated with it.
 */
struct kn_value_t {
	enum kn_value_kind_t kind;

	union {
		kn_boolean_t boolean;
		kn_integer_t integer;
		struct kn_string_t string;
		struct kn_ast_t *ast;
	};
};

/*
 * Creates a new string value.
 *
 * After calling this function, the caller is not permitted to call
 * `kn_string_free` on the string that was passed to this function; doing so
 * would cause a double free when this struct is freed.
 */
struct kn_value_t kn_value_new_string(struct kn_string_t string);

/*
 * Creates a new integer value.
 */
struct kn_value_t kn_value_new_integer(kn_integer_t integer);

/*
 * Creates a new boolean value.
 */
struct kn_value_t kn_value_new_boolean(kn_boolean_t boolean);

/*
 * Creates a new null value.
 */
struct kn_value_t kn_value_new_null(void);

/*
 * Creates a new AST value.
 *
 * The pointer given to this function must be freeable by calling `free`.
 * Additionally, after calling this function, the caller is not permitted to
 * free either the pointer or the underlying ast; doing so would cause a double
 * free when this struct is freed.
 */
struct kn_value_t kn_value_new_ast(struct kn_ast_t *ast);

/*
 * Converts the given value to a string.
 *
 * The returned string must be manually freed via `kn_string_free` after use
 * (just like any other `kn_string_free`), even if the value was already a 
 * string to begin with. 
 *
 * Conversions are defined as follows:
 * - Strings: The string is simply cloned.
 * - Numbers: Converted as you'd expect, with a leading `-` for negative values.
 * - Booleans: True is `true` and false is `false`.
 * - Null: `null`.
 */
struct kn_string_t kn_value_to_string(const struct kn_value_t *value);

/*
 * Converts the given value to a boolean.
 *
 * Conversions are defined as follows:
 * - Strings: The empty string is considered false; everything else is true.
 * - Numbers: Zero is considered false; everything else is true.
 * - Booleans: Simply returned unchanged.
 * - Null: Always false.
 */
kn_boolean_t kn_value_to_boolean(const struct kn_value_t *value);

/*
 * Converts the given value to an integer.
 *
 * Conversions are defined as follows:
 * - Strings: Leading whitespace is stripped, then all the leading digits
 * are converted; if there are no leading digits, it's considered 0.
 * - Numbers: Simply returned unchanged.
 * - Booleans: True is `1` and false is `0`.
 * - Null: Always `0`.
 */
kn_integer_t kn_value_to_integer(const struct kn_value_t *value);

/*
 * Dumps debugging information about the value to stdout.
 *
 * This does not add a trailing newline.
 */
void kn_value_dump(const struct kn_value_t *value);

/*
 * Clones a value.
 *
 * Both the clone and original value will need to be passed to `kn_value_free`.
 */
struct kn_value_t kn_value_clone(const struct kn_value_t *value);

/*
 * Frees the memory associated with a value.
 *
 * This function does not free the pointer itself.
 */
void kn_value_free(struct kn_value_t *value);

#endif /* KN_VALUE_H */
