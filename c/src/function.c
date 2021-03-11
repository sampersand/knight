#include <string.h>  /* memcpy, strlen, strcmp, strndup, strcat */
#include <assert.h>  /* assert */
#include <stdlib.h>  /* rand, exit */
#include <stdbool.h> /* true, false */
#include <stdio.h>   /* getline, stdin, feof, perror, FILE, fread, ferror,
                      * popen, pclose, printf */

#include "env.h"      /* kn_env_set */
#include "knight.h"   /* kn_run */
#include "shared.h"   /* xmalloc, xrealloc, die */
#include "function.h" /* prototypes, kn_value_t, kn_ast_t, all kn_ast functions,
                       * all kn_value functions */

struct kn_value_t kn_fn_assign(const struct kn_ast_t *args) {
	assert(args != NULL);

	struct kn_value_t ret;

	// if it's an identifier, special-case it where we don't evaluate it.
	if (args[0].kind == KN_TT_IDENTIFIER) {
		ret = kn_ast_run(&args[1]);
		kn_env_set(args[0].identifier, kn_value_clone(&ret));
	} else {
		// otherwise, evaluate the expression, convert to a string,
		// and then use that as the identifier.
		struct kn_value_t var = kn_ast_run(&args[0]);
		struct kn_string_t identifier = kn_value_to_string(&var);

		ret = kn_ast_run(&args[1]);
		kn_env_set(identifier.str, kn_value_clone(&ret));

		kn_string_free(&identifier);
		kn_value_free(&var);
	}

	return ret;
}

struct kn_value_t kn_fn_block(const struct kn_ast_t *args) {
	assert(args != NULL);

	// if we're being passed a literal, return that instead.
	if (args[0].kind == KN_TT_VALUE) {
		return kn_value_clone(&args[0].value);
	}

	struct kn_ast_t *ast = xmalloc(sizeof(struct kn_ast_t));

	*ast = kn_ast_clone(&args[0]);

	return kn_value_new_ast(ast);
}

struct kn_value_t kn_fn_while(const struct kn_ast_t *args) {
	assert(args != NULL);

	struct kn_value_t ret = kn_value_new_null();
	struct kn_value_t condition = kn_ast_run(&args[0]);

	while (kn_value_to_boolean(&condition)) {
		kn_value_free(&condition);
		kn_value_free(&ret);

		ret = kn_ast_run(&args[1]);
		condition = kn_ast_run(&args[0]);
	}

	kn_value_free(&condition);
	return ret;
}

struct kn_value_t kn_fn_if(const struct kn_ast_t *args) {
	assert(args != NULL);

	struct kn_value_t condition = kn_ast_run(&args[0]);

	// a little efficiency hack to pick which argument to execute.
	int which_arg = kn_value_to_boolean(&condition) ? 1 : 2;
	kn_value_free(&condition);

	return kn_ast_run(&args[which_arg]);
}

struct kn_value_t kn_fn_and(const struct kn_ast_t *args) {
	assert(args != NULL);

	struct kn_value_t ret = kn_ast_run(&args[0]);

	// execute the RHS if the LHS is true.
	if (kn_value_to_boolean(&ret)) {
		kn_value_free(&ret);
		ret = kn_ast_run(&args[1]);
	}

	return ret;
}

struct kn_value_t kn_fn_or(const struct kn_ast_t *args) {
	assert(args != NULL);

	struct kn_value_t ret = kn_ast_run(&args[0]);

	// execute the RHS if the LHS is false.
	if (!kn_value_to_boolean(&ret)) {
		kn_value_free(&ret);
		ret = kn_ast_run(&args[1]);
	}

	return ret;
}

// TODO: use fgets.
struct kn_value_t kn_fn_prompt(const struct kn_ast_t *args) {
	assert(args != NULL);

	(void) args;

	size_t cap = 0;
	ssize_t slen;
	char *line = NULL;

	// try to read a line from stdin.
	if ((slen = getline(&line, &cap, stdin)) == -1) {
		// if we're at eof, return an emtpy string. otherwise, abort.
		if (feof(stdin)) {
			return kn_value_new_string(kn_string_intern(""));
		} else {
			perror("unable to read line");
		}
	}

	size_t len = (size_t) slen;

	char *ret = xmalloc(len + 1);
	memcpy(ret, line, len);
	ret[len] = '\0';

	return kn_value_new_string(kn_string_new(ret));
}

struct kn_value_t kn_fn_rand(const struct kn_ast_t *args) {
	assert(args != NULL);
	(void) args;

	// note: `kn_init` seeds the random value.
	return kn_value_new_integer((kn_integer_t) rand());
}

struct kn_value_t kn_fn_true(const struct kn_ast_t *args) {
	assert(args != NULL);
	(void) args;

	return kn_value_new_boolean(true);
}

struct kn_value_t kn_fn_false(const struct kn_ast_t *args) {
	assert(args != NULL);
	(void) args;

	return kn_value_new_boolean(false);
}

struct kn_value_t kn_fn_null(const struct kn_ast_t *args) {
	assert(args != NULL);
	(void) args;

	return kn_value_new_null();
}

struct kn_value_t kn_fn_eval(const struct kn_ast_t *args) {
	assert(args != NULL);

	struct kn_value_t toeval = kn_ast_run(&args[0]);
	struct kn_string_t string = kn_value_to_string(&toeval);

	// run the string as if it were its own program, but in the same namespace.
	struct kn_value_t ret = kn_run(string.str);

	kn_string_free(&string);
	kn_value_free(&toeval);

	return ret;
}

struct kn_value_t kn_fn_call(const struct kn_ast_t *args) {
	assert(args != NULL);

	struct kn_value_t block = kn_ast_run(&args[0]);

	// running anything other than a block of code is just a no op.
	if (block.kind != KN_VT_AST) {
		return block;
	}

	struct kn_value_t ret = kn_ast_run(block.ast);

	kn_value_free(&block);

	return ret;
}

// Yay for manual memory management!
struct kn_value_t kn_fn_system(const struct kn_ast_t *args) {
	assert(args != NULL);

	struct kn_value_t arg0 = kn_ast_run(&args[0]);
	struct kn_string_t command = kn_value_to_string(&arg0);
	struct kn_value_t ret;

	FILE *stream = popen(command.str, "r");

	if (stream == NULL) {
		die("unable to execute command '%s'.", command.str);
	}

	size_t cap = 2048;
	size_t len = 0;
	size_t tmp;
	char *result = xmalloc(cap);

	// try to read the entire stream's stdout to `result`.
	while (0 != (tmp = fread(result + len, 1, cap - len, stream))) {
		len += tmp;

		if (len == cap) {
			cap *= 2;
			result = xrealloc(result, cap);
		}
	}

	// Abort if `stream` had an error.
	if (ferror(stream)) {
		die("unable to read command stream");
	}

	result = xrealloc(result, len + 1);
	result[len] = '\0';

	// Abort if we cant close stream.
	if (pclose(stream) == -1) {
		die("unable to close command stream.");
	}

	ret = kn_value_new_string(kn_string_new(result));

	kn_string_free(&command);
	kn_value_free(&arg0);

	return ret;
}

struct kn_value_t kn_fn_quit(const struct kn_ast_t *args) {
	assert(args != NULL);

	struct kn_value_t ecode = kn_ast_run(&args[0]);

	// no need to free the `ecode` value because we're exiting anyways.
	exit((int) kn_value_to_integer(&ecode));
}

struct kn_value_t kn_fn_not(const struct kn_ast_t *args) {
	assert(args != NULL);

	struct kn_value_t value = kn_ast_run(&args[0]);

	struct kn_value_t ret =
		kn_value_new_boolean(!kn_value_to_boolean(&value));

	kn_value_free(&value);

	return ret;
}

struct kn_value_t kn_fn_length(const struct kn_ast_t *args) {
	assert(args != NULL);

	struct kn_value_t arg = kn_ast_run(&args[0]);
	struct kn_string_t string = kn_value_to_string(&arg);

	struct kn_value_t ret = kn_value_new_integer(strlen(string.str));

	kn_string_free(&string);
	kn_value_free(&arg);

	return ret;
}

struct kn_value_t kn_fn_dump(const struct kn_ast_t *args) {
	assert(args != NULL);

	struct kn_value_t ret = kn_ast_run(&args[0]);

	kn_value_dump(&ret);

	return ret;
}

struct kn_value_t kn_fn_output(const struct kn_ast_t *args) {
	assert(args != NULL);

	struct kn_value_t arg = kn_ast_run(&args[0]);
	struct kn_string_t string = kn_value_to_string(&arg);

	// right here we're casting away the const.
	// this is because we might need to replace the penult character
	// with a `\0` if it's a backslash to prevent the printing of a
	// newline. however, we replace it on the next line, so it's ok.
	char *str = (char *) string.str;
	assert(str != NULL);

	size_t len = strlen(str);
	char *penult = &str[len - 1];

	if (len != 0 && *penult == '\\') {
		*penult = '\0'; // replace the trailing `\`
		printf("%s", str);
		*penult = '\\'; // and then restore it.
	} else {
		printf("%s\n", str);
	}

	kn_string_free(&string);

	return arg;
}

static struct kn_value_t kn_fn_add_string(
	struct kn_value_t lhs,
	struct kn_value_t rhs
) {
	assert(lhs.kind == KN_VT_STRING);

	bool is_rhs_not_a_string = rhs.kind != KN_VT_STRING;
	struct kn_string_t rstring =
		is_rhs_not_a_string ? kn_value_to_string(&rhs) : rhs.string;

	size_t lhslen = strlen(lhs.string.str);
	size_t rhslen = strlen(rstring.str);

	if (lhslen == 0) {
		kn_value_free(&lhs);

		if (is_rhs_not_a_string) {
			kn_value_free(&rhs);
			rhs = kn_value_new_string(rstring);
		}

		return rhs;
	} else if (rhslen == 0) {
		kn_value_free(&rhs);

		if (is_rhs_not_a_string) {
			kn_string_free(&rstring);
		}

		return lhs;

	}

	char *ret = xmalloc(lhslen + rhslen + 1);

	memcpy(ret, lhs.string.str, lhslen);
	memcpy(ret + lhslen, rstring.str, rhslen);

	ret[lhslen + rhslen] = '\0';

	kn_value_free(&lhs);
	kn_value_free(&rhs);

	if (is_rhs_not_a_string) {
		kn_string_free(&rstring);
	}

	return kn_value_new_string(kn_string_new(ret));
}

struct kn_value_t kn_fn_add(const struct kn_ast_t *args) {
	assert(args != NULL);

	struct kn_value_t lhs = kn_ast_run(&args[0]);
	struct kn_value_t rhs = kn_ast_run(&args[1]);

	// If lhs is a string, convert both to a string and concat
	if (lhs.kind == KN_VT_STRING) {
		return kn_fn_add_string(lhs, rhs);
	}

	kn_integer_t augend = kn_value_to_integer(&lhs);
	kn_integer_t addend = kn_value_to_integer(&rhs);

	kn_value_free(&lhs);
	kn_value_free(&rhs);

	return kn_value_new_integer(augend + addend);
}

struct kn_value_t kn_fn_sub(const struct kn_ast_t *args) {
	assert(args != NULL);

	struct kn_value_t lhs = kn_ast_run(&args[0]);
	struct kn_value_t rhs = kn_ast_run(&args[1]);

	kn_integer_t minuend = kn_value_to_integer(&lhs);
	kn_integer_t subtrahend = kn_value_to_integer(&rhs);

	kn_value_free(&lhs);
	kn_value_free(&rhs);

	return kn_value_new_integer(minuend - subtrahend);
}

static struct kn_value_t kn_fn_mul_string(
	struct kn_value_t lhs, 
	struct kn_value_t rhs
) {
	assert(lhs.kind == KN_VT_STRING);

	size_t lhslen = strlen(lhs.string.str);
	size_t amnt = kn_value_to_integer(&rhs);
	kn_value_free(&rhs); // we don't need it anymore, we just need its amnt.

	// if we have an empty string, return early.
	if (lhslen == 0 || amnt == 1) {
		return lhs;
	} else if (amnt == 0) {
		kn_value_free(&lhs);
		return kn_value_new_string(kn_string_intern(""));
	}

	char *string = xmalloc(1 + lhslen * amnt);
	char *ptr = string;

	for (; amnt != 0; --amnt) {
		memcpy(ptr, lhs.string.str, lhslen);
		ptr += lhslen;
	}

	*ptr = '\0';

	kn_value_free(&lhs);
	return kn_value_new_string(kn_string_new(string));
}

struct kn_value_t kn_fn_mul(const struct kn_ast_t *args) {
	assert(args != NULL);

	struct kn_value_t lhs = kn_ast_run(&args[0]);
	struct kn_value_t rhs = kn_ast_run(&args[1]);

	if (lhs.kind == KN_VT_STRING) {
		return kn_fn_mul_string(lhs, rhs);
	}

	kn_integer_t multiplier = kn_value_to_integer(&lhs);
	kn_integer_t multiplicand = kn_value_to_integer(&rhs);

	kn_value_free(&lhs);
	kn_value_free(&rhs);

	return kn_value_new_integer(multiplier * multiplicand);
}

struct kn_value_t kn_fn_div(const struct kn_ast_t *args) {
	assert(args != NULL);

	struct kn_value_t lhs = kn_ast_run(&args[0]);
	struct kn_value_t rhs = kn_ast_run(&args[1]);

	kn_integer_t dividend = kn_value_to_integer(&lhs);
	kn_integer_t divisor = kn_value_to_integer(&rhs);

	if (divisor == 0) {
		die("attempted to divide by zero");
	}

	kn_value_free(&lhs);
	kn_value_free(&rhs);

	return kn_value_new_integer(dividend / divisor);
}

struct kn_value_t kn_fn_mod(const struct kn_ast_t *args) {
	assert(args != NULL);

	struct kn_value_t lhs = kn_ast_run(&args[0]);
	struct kn_value_t rhs = kn_ast_run(&args[1]);

	kn_integer_t number = kn_value_to_integer(&lhs);
	kn_integer_t base = kn_value_to_integer(&rhs);

	if (base == 0) {
		die("attempted to modulo by zero");
	}

	kn_value_free(&lhs);
	kn_value_free(&rhs);

	return kn_value_new_integer(number % base);
}

struct kn_value_t kn_fn_pow(const struct kn_ast_t *args) {
	assert(args != NULL);

	struct kn_value_t lhs = kn_ast_run(&args[0]);
	struct kn_value_t rhs = kn_ast_run(&args[1]);

	kn_integer_t result = 1;
	kn_integer_t base = kn_value_to_integer(&lhs);
	kn_integer_t exponent = kn_value_to_integer(&rhs);

	// there's no builtin way to do integer exponentiation, so we have to
	// do it manually.
	if (base == 1) {
		result = 1;
	} else if (base == -1) {
		result = exponent & 1 ? -1 : 1; 
	} else if (exponent == 1) {
		result = base;
	} else if (exponent == 0) {
		result = 1;
	} else if (exponent < 0) {
		result = 0; // already handled the `base == -1` case
	} else {
		result = 1;

		for (; exponent > 0; --exponent) {
			result *= base;
		}
	}

	kn_value_free(&lhs);
	kn_value_free(&rhs);

	return kn_value_new_integer(result);
}

struct kn_value_t kn_fn_eql(const struct kn_ast_t *args) {
	assert(args != NULL);

	struct kn_value_t lhs = kn_ast_run(&args[0]);
	struct kn_value_t rhs = kn_ast_run(&args[1]);
	bool is_eql;

	// if their kinds aren't equal, they're by definition not equal.
	if (lhs.kind != rhs.kind) {
		is_eql = false;
		goto free_and_return;
	}

	switch(lhs.kind) {
	case KN_VT_NULL:
		is_eql = true;
		break;

	case KN_VT_BOOLEAN:
		is_eql = lhs.boolean == rhs.boolean;
		break;

	case KN_VT_INTEGER:
		is_eql = lhs.integer == rhs.integer;
		break;

	case KN_VT_STRING:
		is_eql = strcmp(lhs.string.str, rhs.string.str) == 0;
		break;

	case KN_VT_AST:
		// ASTs are only equal if they're the _exact same_ object.
		if (lhs.ast->kind != rhs.ast->kind) {
			is_eql = false;
			goto free_and_return;
		}

		// we should never have an AST of values...
		assert(lhs.ast->kind == KN_TT_IDENTIFIER ||
		       lhs.ast->kind == KN_TT_FUNCTION);

		if (lhs.ast->kind == KN_TT_IDENTIFIER) {
			is_eql = lhs.ast->identifier == rhs.ast->identifier;
		} else {
			is_eql = lhs.ast->arguments == rhs.ast->arguments;

			// sanity check because the function should be the same
			// for identical arguments.
			assert(!is_eql ||
				lhs.ast->function == rhs.ast->function);
		}

		break;

	default:
		bug("unknown kind '%d'", lhs.kind);
	}

free_and_return:
	kn_value_free(&lhs);
	kn_value_free(&rhs);

	return kn_value_new_boolean(is_eql);
}

static int kn_value_cmp(struct kn_value_t lhs, struct kn_value_t rhs) {
	switch(lhs.kind) {
	case KN_VT_NULL:
		die("cannot compare NULL");

	case KN_VT_BOOLEAN:
		return lhs.boolean - kn_value_to_boolean(&rhs);

	case KN_VT_INTEGER:
		return lhs.integer - kn_value_to_integer(&rhs);

	case KN_VT_STRING: {
		struct kn_string_t rstring = kn_value_to_string(&rhs);
		int cmp = strcmp(lhs.string.str, rstring.str);
		kn_string_free(&rstring);
		return cmp;
	}

	case KN_VT_AST: {
		struct kn_value_t evaluated = kn_ast_run(lhs.ast);
		int cmp = kn_value_cmp(evaluated, rhs);
		kn_value_free(&evaluated);
		return cmp;
	}

	default:
		bug("unknown kind '%d'", lhs.kind);
	}
}

struct kn_value_t kn_fn_lth(const struct kn_ast_t *args) {
	assert(args != NULL);

	struct kn_value_t lhs = kn_ast_run(&args[0]);
	struct kn_value_t rhs = kn_ast_run(&args[1]);

	struct kn_value_t ret =
		kn_value_new_boolean(kn_value_cmp(lhs, rhs) < 0);

	kn_value_free(&lhs);
	kn_value_free(&rhs);

	return ret;
}

struct kn_value_t kn_fn_gth(const struct kn_ast_t *args) {
	assert(args != NULL);

	struct kn_value_t lhs = kn_ast_run(&args[0]);
	struct kn_value_t rhs = kn_ast_run(&args[1]);

	struct kn_value_t ret =
		kn_value_new_boolean(kn_value_cmp(lhs, rhs) > 0);

	kn_value_free(&lhs);
	kn_value_free(&rhs);

	return ret;
}

struct kn_value_t kn_fn_then(const struct kn_ast_t *args) {
	assert(args != NULL);

	struct kn_value_t lhs = kn_ast_run(&args[0]);
	kn_value_free(&lhs);

	return kn_ast_run(&args[1]);
}

struct kn_value_t kn_fn_get(const struct kn_ast_t *args) {
	assert(args != NULL);

	struct kn_value_t arg0 = kn_ast_run(&args[0]);
	struct kn_value_t arg1 = kn_ast_run(&args[1]);
	struct kn_value_t arg2 = kn_ast_run(&args[2]);
	struct kn_value_t ret;

	struct kn_string_t string = kn_value_to_string(&arg0);
	size_t start = (size_t) kn_value_to_integer(&arg1);
	size_t amnt = (size_t) kn_value_to_integer(&arg2);

	size_t length = strlen(string.str);

	if (length - 1 < start) {
		ret = kn_value_new_string(kn_string_intern(""));
	} else {
		char *substr = strndup(string.str + start, amnt);

		if (substr == NULL) {
			die("substring creation failed");
		}

		ret = kn_value_new_string(kn_string_new(substr));
	}

	kn_string_free(&string);
	kn_value_free(&arg2);
	kn_value_free(&arg1);
	kn_value_free(&arg0);

	return ret;
}

struct kn_value_t kn_fn_set(const struct kn_ast_t *args) {
	assert(args != NULL);

	struct kn_value_t arg0 = kn_ast_run(&args[0]);
	struct kn_value_t arg1 = kn_ast_run(&args[1]);
	struct kn_value_t arg2 = kn_ast_run(&args[2]);
	struct kn_value_t arg3 = kn_ast_run(&args[3]);
	struct kn_value_t ret;

	struct kn_string_t string = kn_value_to_string(&arg0);
	size_t start = (size_t) kn_value_to_integer(&arg1);
	size_t amnt = (size_t) kn_value_to_integer(&arg2);
	size_t length = strlen(string.str);
	struct kn_string_t substr = kn_value_to_string(&arg3);

	// if it's out of bounds, die.
	if (length < start) {
		die("index '%zu' out of bounds (length=%zu)", start, length);
	}

	// this could be made more efficient.
	char *retstr = xmalloc(length + strlen(substr.str) + 1);
	memcpy(retstr, string.str, start);
	retstr[start] = '\0';
	strcat(retstr + start, substr.str);
	strcat(retstr + start + strlen(substr.str), string.str + start + amnt);

	ret = kn_value_new_string(kn_string_new(retstr));

	kn_string_free(&substr);
	kn_string_free(&string);
	kn_value_free(&arg3);
	kn_value_free(&arg2);
	kn_value_free(&arg1);
	kn_value_free(&arg0);

	return ret;
}

#define DECLARE_FUNCTION(name, arity, pointer) [name] = { arity, pointer, name }

static struct kn_function_t FUNCTIONS[0xff] = {
	DECLARE_FUNCTION('P', 0, kn_fn_prompt),
	DECLARE_FUNCTION('R', 0, kn_fn_rand),
	DECLARE_FUNCTION('T', 0, kn_fn_true),
	DECLARE_FUNCTION('F', 0, kn_fn_false),
	DECLARE_FUNCTION('N', 0, kn_fn_null),

	DECLARE_FUNCTION('B', 1, kn_fn_block),
	DECLARE_FUNCTION('E', 1, kn_fn_eval),
	DECLARE_FUNCTION('C', 1, kn_fn_call),
	DECLARE_FUNCTION('`', 1, kn_fn_system),
	DECLARE_FUNCTION('Q', 1, kn_fn_quit),
	DECLARE_FUNCTION('!', 1, kn_fn_not),
	DECLARE_FUNCTION('L', 1, kn_fn_length),
	DECLARE_FUNCTION('D', 1, kn_fn_dump),
	DECLARE_FUNCTION('O', 1, kn_fn_output),

	DECLARE_FUNCTION('+', 2, kn_fn_add),
	DECLARE_FUNCTION('-', 2, kn_fn_sub),
	DECLARE_FUNCTION('*', 2, kn_fn_mul),
	DECLARE_FUNCTION('/', 2, kn_fn_div),
	DECLARE_FUNCTION('%', 2, kn_fn_mod),
	DECLARE_FUNCTION('^', 2, kn_fn_pow),
	DECLARE_FUNCTION('?', 2, kn_fn_eql),
	DECLARE_FUNCTION('<', 2, kn_fn_lth),
	DECLARE_FUNCTION('>', 2, kn_fn_gth),
	DECLARE_FUNCTION(';', 2, kn_fn_then),
	DECLARE_FUNCTION('=', 2, kn_fn_assign),
	DECLARE_FUNCTION('W', 2, kn_fn_while),
	DECLARE_FUNCTION('&', 2, kn_fn_and),
	DECLARE_FUNCTION('|', 2, kn_fn_or),

	DECLARE_FUNCTION('I', 3, kn_fn_if),
	DECLARE_FUNCTION('G', 3, kn_fn_get),

	DECLARE_FUNCTION('S', 4, kn_fn_set),
};

const struct kn_function_t *kn_fn_fetch(char name) {
	return FUNCTIONS[(int) name].func == NULL ? NULL : &FUNCTIONS[(int) name];
}

void kn_fn_register_func(struct kn_function_t func) {
	FUNCTIONS[(int) func.name] = func;
}
