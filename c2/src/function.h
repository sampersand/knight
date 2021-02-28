#ifndef FUNCTION_H
#define FUNCTION_H

struct value_t;

struct function_t {
	struct value_t (*func)(const struct value_t*);
	unsigned arity;
};

struct function_t fn_prompt;
struct function_t fn_random;

struct function_t fn_eval;
struct function_t fn_block;
struct function_t fn_call;
struct function_t fn_system;
struct function_t fn_quit;
struct function_t fn_not;
struct function_t fn_length;
struct function_t fn_dump;
struct function_t fn_output;

struct function_t fn_add;
struct function_t fn_sub;
struct function_t fn_mul;
struct function_t fn_div;
struct function_t fn_mod;
struct function_t fn_eql;
struct function_t fn_lth;
struct function_t fn_gth;
struct function_t fn_and;
struct function_t fn_or;
struct function_t fn_then;
struct function_t fn_assign;
struct function_t fn_while;

struct function_t fn_if;
struct function_t fn_get;

struct function_t fn_set;

#endif
