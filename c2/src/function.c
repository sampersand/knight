#include "function.h"
#include "value.h"

#include <string.h>  /* memcpy, strlen, strcmp, strndup, strcat */
#include <assert.h>  /* assert */
#include <stdlib.h>  /* rand, exit */
#include <stdbool.h> /* true, false */
#include <stdio.h>   /* getline, stdin, feof, perror, FILE, fread, ferror,
                      * popen, pclose, printf */
#include <inttypes.h>

// this is a workaround and i only use one argument.
#define DECLARE_FUNCTION(_name, _arity, ...) \
	static struct value_t fn_##_name##_func(const struct value_t *args) { \
		assert(args != NULL); \
		__VA_ARGS__ \
	} \
	struct function_t fn_##_name = (struct function_t) { \
		.func = fn_##_name##_func, \
		.arity = _arity \
	}

DECLARE_FUNCTION(prompt, 0, {
	(void) args;
	size_t cap = 0;
	ssize_t slen;
	char *line = NULL;

	// try to read a line from stdin.
	if ((slen = getline(&line, &cap, stdin)) == -1) {
		// if we're at eof, return an emtpy string. otherwise, abort.
		if (feof(stdin)) {
			return EMPTY_STRING;
		} else {
			perror("unable to read line");
		}
	}

	size_t len = (size_t) slen;

	char *ret = xmalloc(len + 1);
	memcpy(ret, line, len);
	ret[len] = '\0';

	return value_new_string(ret);
});

// struct function_t fn_random;

// struct function_t fn_eval;
// struct function_t fn_block;
// struct function_t fn_call;
// struct function_t fn_system;
// struct function_t fn_quit;
// struct function_t fn_not;
// struct function_t fn_length;
// struct function_t fn_dump;
// struct function_t fn_output;

// struct function_t fn_add;
// struct function_t fn_sub;
// struct function_t fn_mul;
// struct function_t fn_div;
// struct function_t fn_mod;
// struct function_t fn_eql;
// struct function_t fn_lth;
// struct function_t fn_gth;
// struct function_t fn_and;
// struct function_t fn_or;
// struct function_t fn_then;
// struct function_t fn_assign;
// struct function_t fn_while;

// struct function_t fn_if;
// struct function_t fn_get;

// struct function_t fn_set;
