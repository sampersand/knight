#ifndef FUNCTION_H
#define FUNCTION_H

#include "value.h"

struct kn_function_t {
	kn_value_t (*ptr)(const kn_value_t*);
	unsigned arity;
	char name;
};


void kn_function_startup(void);

/* ARITY ZERO */
#ifndef KN_EMBEDDED
extern struct kn_function_t kn_fn_prompt;
#endif

extern struct kn_function_t kn_fn_random;

/* ARITY ONE */
#ifdef KN_EXT_VALUE
extern struct kn_function_t kn_fn_value;
#endif
extern struct kn_function_t kn_fn_eval;
extern struct kn_function_t kn_fn_block;
extern struct kn_function_t kn_fn_call;
extern struct kn_function_t kn_fn_not;
extern struct kn_function_t kn_fn_length;

#ifndef KN_EMBEDDED
extern struct kn_function_t kn_fn_system;
extern struct kn_function_t kn_fn_quit;
extern struct kn_function_t kn_fn_dump;
extern struct kn_function_t kn_fn_output;
#endif

/* ARITY TWO */
extern struct kn_function_t kn_fn_add;
extern struct kn_function_t kn_fn_sub;
extern struct kn_function_t kn_fn_mul;
extern struct kn_function_t kn_fn_div;
extern struct kn_function_t kn_fn_mod;
extern struct kn_function_t kn_fn_pow;
extern struct kn_function_t kn_fn_eql;
extern struct kn_function_t kn_fn_lth;
extern struct kn_function_t kn_fn_gth;
extern struct kn_function_t kn_fn_and;
extern struct kn_function_t kn_fn_or;
extern struct kn_function_t kn_fn_then;
extern struct kn_function_t kn_fn_assign;
extern struct kn_function_t kn_fn_while;

/* ARITY THREE */
extern struct kn_function_t kn_fn_if;
extern struct kn_function_t kn_fn_get;

/* ARITY FOUR */
extern struct kn_function_t kn_fn_set;

#endif
