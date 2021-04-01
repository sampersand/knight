#ifndef KN_FUNCTION_H
#define KN_FUNCTION_H

#include "value.h" /* kn_value */

/*
 * This struct is used to keep track of all data relevant for a Knight function.
 */
struct kn_function {
	/*
	 * A pointer to the function. It will take exactly `arity` arguments.
	 */
	kn_value (*func)(const kn_value *args);

	/*
	 * The number of arguments that `func` should expect.
	 */
	unsigned arity;

	/*
	 * The name of the function; used when debugging.
	 */
	char name;
};

/*
 * Initializes all relevant data for functions.
 *
 * This should be called before any `kn_function.func` methods are called.
 */
void kn_function_startup(void);

/*
 * Declares a function with the given function name, arity, and char name.
 */
#define KN_FUNCTION_DECLARE(func_, arity_, name_) \
	static kn_value kn_fn_##func_##_function(const kn_value *); \
	const struct kn_function kn_fn_##func_ = { \
		.func = kn_fn_##func_##_function, \
		.arity = arity_, \
		.name = name_ \
	}; \
	static kn_value kn_fn_##func_##_function(const kn_value *args)


/******************************************************************************
 * The following are all of the different types of functions within Knight.   *
 * For details on what each function does, see the specs---these all conform. *
 ******************************************************************************/

/**
 * 4.1 Arity 0
 *
 * Note that the 4.1.1 (`TRUE`), 4.1.2 (`FALSE`) and 4.1.3 (`NULL`) aren't
 * implemented as functions, but rather literals.
 **/

/* 4.1.4 PROMPT */
extern const struct kn_function kn_fn_prompt;

/* 4.1.5 RANDOM */
extern const struct kn_function kn_fn_random;

/**
 * 4.2 Arity 1
 *
 * Note that the 4.2.1 (`:`) is treated as whitespace, and as such has no
 * associated function.
 **/

/* 4.2.2 EVAL */
extern const struct kn_function kn_fn_eval;

/* 4.2.3 BLOCK */
extern const struct kn_function kn_fn_block;

/* 4.2.4 CALL */
extern const struct kn_function kn_fn_call;

/* 4.2.5 ` */
extern const struct kn_function kn_fn_system;

/* 4.2.6 QUIT */
extern const struct kn_function kn_fn_quit;

/* 4.2.7 ! */
extern const struct kn_function kn_fn_not;

/* 4.2.8 LENGTH */
extern const struct kn_function kn_fn_length;

/* 4.2.9 DUMP */
extern const struct kn_function kn_fn_dump;

/* 4.2.10 OUTPUT */
extern const struct kn_function kn_fn_output;

#ifdef KN_EXT_NEGATE
/*
 * An extension which simply negates its argument, converting it to a number if
 * need be.
 */
extern const struct kn_function kn_fn_negate;
#endif /* KN_EXT_NEGATE */

#ifdef KN_EXT_VALUE
/*
 * An extension function that converts its argument to a string, and then uses
 * the string's value as an identifier to look it up.
 *
 * `VALUE(+ "a" 23)` is simply equivalent to `EVAL(+ "a" 23)`, except the
 * parsing step of `EVAL` is skipped.
 *
 * Any lookups of non-variable-names (eg `VALUE(0)`) will simply terminate the
 * program like any unknown variable lookup would.
 */
extern const struct kn_function kn_fn_value;
#endif /* KN_EXT_VALUE */

/**
 *
 * 4.3 Arity 2
 *
 **/

/* 4.3.1 + */
extern const struct kn_function kn_fn_add;

/* 4.3.2 - */
extern const struct kn_function kn_fn_sub;

/* 4.3.3 * */
extern const struct kn_function kn_fn_mul;

/* 4.3.4 / */
extern const struct kn_function kn_fn_div;

/* 4.3.5 % */
extern const struct kn_function kn_fn_mod;

/* 4.3.6 ^ */
extern const struct kn_function kn_fn_pow;

/* 4.3.7 < */
extern const struct kn_function kn_fn_lth;

/* 4.3.8 > */
extern const struct kn_function kn_fn_gth;

/* 4.3.9 ? */
extern const struct kn_function kn_fn_eql;

/* 4.3.10 & */
extern const struct kn_function kn_fn_and;

/* 4.3.11 | */
extern const struct kn_function kn_fn_or;

/* 4.3.12 ; */
extern const struct kn_function kn_fn_then;

/* 4.3.13 = */
extern const struct kn_function kn_fn_assign;

/* 4.3.14 WHILE */
extern const struct kn_function kn_fn_while;

/**
 *
 * 4.4 Arity 3
 *
 **/

/* 4.4.1 IF */
extern const struct kn_function kn_fn_if;

/* 4.4.2 GET */
extern const struct kn_function kn_fn_get;

/**
 *
 * 4.5 Arity 4
 *
 **/

/* 4.5.1 SUBSTITUTE */
extern const struct kn_function kn_fn_substitute;

#endif /* !KN_FUNCTION_H */
