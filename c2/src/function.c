#include "function.h"
#include "value.h"
#include <assert.h>


// this is a workaround and i only use one argument.
#define DECLARE_FUNCTION(_func, _arity, _name, ...) \
	static kn_value_t fn_##_func##_func(const kn_value_t *args) { \
		assert(args != NULL); \
		__VA_ARGS__ \
	} \
	struct kn_function_t kn_fn_##_func = (struct kn_function_t) { \
		.ptr = fn_##_func##_func, \
		.arity = _arity, \
		.name = _name \
	}

DECLARE_FUNCTION(dump, 1, 'D', {
	kn_value_t arg = kn_value_run(args[0]);
	kn_value_dump(arg);
	return arg;
});

DECLARE_FUNCTION(add, 2, '+', {
	(void) args;

	return (123 << 1) | 1;
});

struct kn_function_t kn_fn_prompt;
struct kn_function_t kn_fn_random;

struct kn_function_t kn_fn_eval;
struct kn_function_t kn_fn_block;
struct kn_function_t kn_fn_call;
struct kn_function_t kn_fn_system;
struct kn_function_t kn_fn_quit;
struct kn_function_t kn_fn_not;
struct kn_function_t kn_fn_length;
struct kn_function_t kn_fn_dump;
struct kn_function_t kn_fn_output;

struct kn_function_t kn_fn_add;
struct kn_function_t kn_fn_sub;
struct kn_function_t kn_fn_mul;
struct kn_function_t kn_fn_div;
struct kn_function_t kn_fn_mod;
struct kn_function_t kn_fn_pow;
struct kn_function_t kn_fn_eql;
struct kn_function_t kn_fn_lth;
struct kn_function_t kn_fn_gth;
struct kn_function_t kn_fn_and;
struct kn_function_t kn_fn_or;
struct kn_function_t kn_fn_then;
struct kn_function_t kn_fn_assign;
struct kn_function_t kn_fn_while;

struct kn_function_t kn_fn_if;
struct kn_function_t kn_fn_get;

struct kn_function_t kn_fn_set;

#endif
