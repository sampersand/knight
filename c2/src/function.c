#include <string.h>  /* memcpy, strlen, strcmp, strndup, strcat */
#include <assert.h>  /* assert */
#include <stdlib.h>  /* rand, exit */
#include <stdbool.h> /* true, false */
#include <stdio.h>   /* getline, stdin, feof, perror, FILE, fread, ferror,
                      * popen, pclose, printf */
#include <time.h>

#include "shared.h"
#include "function.h"
#include "knight.h"
#include "value.h"
#include "env.h"

void kn_function_init(void) {
	srand(time(NULL));
}

#define DECLARE_FUNCTION(func_, arity_, name_) \
	static kn_value_t fn_##func_##_function(const kn_value_t *); \
	struct kn_function_t kn_fn_##func_ = (struct kn_function_t) { \
		.ptr = fn_##func_##_function, \
		.arity = arity_, \
		.name = name_ \
	}; \
	static kn_value_t fn_##func_##_function(const kn_value_t *args)

#ifndef KN_EMBEDDED
DECLARE_FUNCTION(prompt, 0, 'P') {
	(void) args;

	size_t cap = 0;
	ssize_t slen;
	char *line = NULL;

	// try to read a line from stdin.
	if ((slen = getline(&line, &cap, stdin)) == -1) {
		// if we're at eof, return an emtpy string. otherwise, abort.
		if (feof(stdin))
			return kn_value_new_string(&KN_STRING_EMPTY);
		else
			perror("unable to read line");
	}

	size_t len = (size_t) slen;

	char *ret = xmalloc(len + 1);
	memcpy(ret, line, len);
	ret[len] = '\0';

	return kn_value_new_string(kn_string_new(ret));
}
#endif /* KN_EMBEDDED */


DECLARE_FUNCTION(rand, 0, 'R') {
	(void) args;

	return kn_value_new_number((kn_number_t) rand());
}

#ifdef KN_EXT_VALUE
DECLARE_FUNCTION(value, 1, 'V') {
	struct kn_string_t *string = kn_value_to_string(args[0]);
	struct kn_variable_t *variable = kn_env_fetch(string->str, false);

	kn_string_free(string);

	if (variable->value == KN_UNDEFINED)
		die("undefined variable '%s'", variable->str);

	return kn_value_clone(variable->value);
}
#endif

DECLARE_FUNCTION(eval, 1, 'E') {
	struct kn_string_t *string = kn_value_to_string(args[0]);
	kn_value_t ret = kn_run(string->str);

	kn_string_free(string);

	return ret;
}

DECLARE_FUNCTION(block, 1, 'B') {
	return kn_value_clone(args[0]);
}

DECLARE_FUNCTION(call, 1, 'C') {
	kn_value_t arg0 = kn_value_run(args[0]);
	kn_value_t ret = kn_value_run(arg0);

	kn_value_free(arg0);
	return ret;
}

#ifndef KN_EMBEDDED
DECLARE_FUNCTION(system, 1, '`') {
	struct kn_string_t *command;
	FILE *stream;
	size_t cap, len, tmp;
	char *result;

	command = kn_value_to_string(args[0]);
	stream = popen(command->str, "r");

	if (stream == NULL)
		die("unable to execute command '%s'.", command->str);

	kn_string_free(command);

	cap = 2048;
	len = 0;
	result = xmalloc(cap);

	// try to read the entire stream's stdout to `result`.
	while (0 != (tmp = fread(result + len, 1, cap - len, stream))) {
		len += tmp;

		if (len == cap)
			result = xrealloc(result, cap *= 2);
	}

	// Abort if `stream` had an error.
	if (ferror(stream))
		die("unable to read command stream");

	result = xrealloc(result, len + 1);
	result[len] = '\0';

	// Abort if we cant close stream.
	if (pclose(stream) == -1)
		die("unable to close command stream.");

	return kn_value_new_string(kn_string_emplace(result, len));
}
#endif /* KN_EMBEDDED */

#ifndef KN_EMBEDDED
DECLARE_FUNCTION(quit, 1, 'Q') {
	exit((int) kn_value_to_number(args[0]));
}
#endif /* KN_EMBEDDED */

DECLARE_FUNCTION(not, 1, '!') {
	return kn_value_new_boolean(!kn_value_to_boolean(args[0]));
}

DECLARE_FUNCTION(length, 1 ,'L') {
	struct kn_string_t *string = kn_value_to_string(args[0]);
	size_t length = string->length;

	kn_string_free(string);

	return kn_value_new_number((kn_number_t) length);
}

#ifndef KN_EMBEDDED
DECLARE_FUNCTION(dump, 1 ,'D') {
	kn_value_t ret = kn_value_run(args[0]);
	kn_value_dump(ret);
	return ret;
}
#endif /* KN_EMBEDDED */

#ifndef KN_EMBEDDED
DECLARE_FUNCTION(output, 1, 'O') {
	struct kn_string_t *string = kn_value_to_string(args[0]);

	if (!string->length)
		goto print_newline;

	// right here we're casting away the const.
	// this is because we might need to replace the penult character
	// with a `\0` if it's a backslash to prevent the printing of a
	// newline. however, we replace it later, so it's ok. (assuming the
	// input string doesn't have a `\\` in it, but no interned strings do.)
	char *penult = (char *) &string->str[string->length - 1];
	if (*penult == '\\') {
		*penult = '\0'; // replace the trailing `\`...
		puts(string->str);
		*penult = '\\'; // ...and then restore it.
	} else {
		puts(string->str);
print_newline:
		puts("\n");
	}

	kn_string_free(string);

	return KN_NULL;
}
#endif /* KN_EMBEDDED */

static kn_value_t add_string(struct kn_string_t *lhs, struct kn_string_t *rhs) {
	// note that all length-zero strings are always interned, so no need
	// to free
	if (lhs->length == 0)
		return kn_value_new_string(rhs);

	if (rhs->length == 0)
		return kn_value_new_string(lhs);

	size_t length = lhs->length + rhs->length;
	char *str = xmalloc(length + 1);

	memcpy(str, lhs->str, lhs->length);
	memcpy(str + lhs->length, rhs->str, rhs->length);

	str[length] = '\0';

	kn_string_free(lhs);
	kn_string_free(rhs);

	return kn_value_new_string(kn_string_emplace(str, length));
}

DECLARE_FUNCTION(add, 2, '+') {
	kn_value_t lhs = kn_value_run(args[0]);

	// If lhs is a string, convert both to a string and concat
	if (kn_value_is_string(lhs)) {
		return add_string(
			kn_value_as_string(lhs),
			kn_value_to_string(args[1]));
	}

	kn_number_t lhs_num = kn_value_as_number(lhs);
	kn_number_t rhs_num = kn_value_to_number(args[1]);

	return kn_value_new_number(lhs_num + rhs_num);
}

DECLARE_FUNCTION(sub, 2, '-') {
	kn_value_t lhs = kn_value_run(args[0]);

	kn_number_t lhs_num = kn_value_as_number(lhs);
	kn_number_t rhs_num = kn_value_to_number(args[1]);

	return kn_value_new_number(lhs_num - rhs_num);
}

static kn_value_t mul_string(struct kn_string_t *lhs, size_t amnt) {
	if (!lhs->length || amnt == 0)
		return kn_value_new_string(&KN_STRING_EMPTY);

	if (amnt == 1)
		return kn_value_new_string(kn_string_clone(lhs));

	size_t length = lhs->length * amnt;
	char *str = xmalloc(length + 1);

	for (char *ptr = str; amnt != 0; --amnt, ptr += lhs->length)
		memcpy(ptr, lhs->str, lhs->length);

	str[length] = '\0';

	return kn_value_new_string(kn_string_emplace(str, length));
}

DECLARE_FUNCTION(mul, 2, '*') {
	kn_value_t lhs = kn_value_run(args[0]);

	// If lhs is a string, convert both to a string and concat
	if (kn_value_is_string(lhs)) {
		return mul_string(
			kn_value_as_string(lhs),
			(size_t) kn_value_to_number(args[1]));
	}
	
	kn_number_t lhs_num = kn_value_as_number(lhs);
	kn_number_t rhs_num = kn_value_to_number(args[1]);

	return kn_value_new_number(lhs_num * rhs_num);
}

DECLARE_FUNCTION(div, 2, '/') {
	kn_value_t lhs = kn_value_run(args[0]);

	kn_number_t dividend = kn_value_as_number(lhs);
	kn_number_t divisor = kn_value_to_number(args[1]);

#ifndef NDEBUG
	if (divisor == 0)
		die("attempted to divide by zero");
#endif

	return kn_value_new_number(dividend / divisor);
}

DECLARE_FUNCTION(mod, 2, '%') {
	kn_value_t lhs = kn_value_run(args[0]);

	kn_number_t number = kn_value_as_number(lhs);
	kn_number_t base = kn_value_to_number(args[1]);

#ifndef NDEBUG
	if (base == 0)
		die("attempted to modulo by zero");
#endif

	return kn_value_new_number(number % base);
}

// TODO: remove unneeded stuff to comply with the specs.
// (or should the specs be updated?)
DECLARE_FUNCTION(pow, 2, '^') {
	kn_number_t result;
	kn_number_t base = kn_value_to_number(args[0]);
	kn_number_t exponent = kn_value_to_number(args[1]);

	// there's no builtin way to do integer exponentiation, so we have to
	// do it manually.
	if (base == 1) result = 1;
	else if (base == -1) result = exponent & 1 ? -1 : 1;
	else if (exponent == 1) result = base;
	else if (exponent == 0) result = 1;
	else if (exponent < 0) result = 0; // already handled `base == -1`
	else for (result = 1; exponent > 0; --exponent)
		result *= base;

	return kn_value_new_number(result);
}

DECLARE_FUNCTION(eql, 2, '?') {
	kn_value_t lhs = kn_value_run(args[0]);
	kn_value_t rhs = kn_value_run(args[1]);
	bool eql;

	assert(lhs != KN_UNDEFINED);
	assert(rhs != KN_UNDEFINED);

	if ((eql = (lhs == rhs)))
		goto free_and_return;

	if (!(eql = (kn_value_is_string(lhs) && kn_value_is_string(rhs))))
		goto free_and_return;

	struct kn_string_t *lstr = kn_value_as_string(lhs);
	struct kn_string_t *rstr = kn_value_as_string(rhs);

	eql = lstr->length == rstr->length && !strcmp(lstr->str, rstr->str);

free_and_return:
	kn_value_free(lhs);
	kn_value_free(rhs);

	return kn_value_new_boolean(eql);
}

DECLARE_FUNCTION(lth, 2, '<') {
	kn_value_t lhs = kn_value_run(args[0]);
	bool less;

	if (kn_value_is_string(lhs)) {
		struct kn_string_t *lstr = kn_value_as_string(lhs);
		struct kn_string_t *rstr = kn_value_to_string(args[1]);

		less = strcmp(lstr->str, rstr->str) < 0;

		kn_string_free(lstr);
		kn_string_free(rstr);
	} else if (kn_value_is_number(lhs)) {
		less = kn_value_as_number(lhs) < kn_value_to_number(args[1]);
	} else {
		assert(kn_value_is_boolean(lhs));

		less = kn_value_to_boolean(args[1]) && lhs == KN_FALSE;
	}

	return kn_value_new_boolean(less);
}

DECLARE_FUNCTION(gth, 2, '>') {
	kn_value_t lhs = kn_value_run(args[0]);
	bool more;

	if (kn_value_is_string(lhs)) {
		struct kn_string_t *lstr = kn_value_as_string(lhs);
		struct kn_string_t *rstr = kn_value_to_string(args[1]);

		more = strcmp(lstr->str, rstr->str) > 0;

		kn_string_free(lstr);
		kn_string_free(rstr);
	} else if (kn_value_is_number(lhs)) {
		more = kn_value_as_number(lhs) > kn_value_to_number(args[1]);
	} else {
		assert(kn_value_is_boolean(lhs));

		more = !kn_value_to_boolean(args[1]) && lhs == KN_TRUE;
	}

	return kn_value_new_boolean(less);
}

DECLARE_FUNCTION(and, 2, '&') {
	kn_value_t lhs = kn_value_run(args[0]);

	// execute the RHS if the LHS is true.
	if (!kn_value_to_boolean(lhs))
		return lhs;

	// DEBUG(__FILE__ " %d\n", __LINE__);
	kn_value_free(lhs);
	return kn_value_run(args[1]);
}

DECLARE_FUNCTION(or, 2, '|') {
	kn_value_t lhs = kn_value_run(args[0]);

	// execute the RHS if the LHS is true.
	if (kn_value_to_boolean(lhs))
		return lhs;

	// DEBUG(__FILE__ " %d\n", __LINE__);
	kn_value_free(lhs);
	return kn_value_run(args[1]);
}

DECLARE_FUNCTION(then, 2, ';') {
#ifndef DYAMIC_THEN_ARGC
	kn_value_t val = kn_value_run(args[0]);
	// DEBUG(__FILE__ " %d(%p)\n", __LINE__, val & ~0b111);
	kn_value_free(val);

	return kn_value_run(args[1]);
#else
	kn_value_t ret;
	unsigned i = 0;

	goto inner;

	do {
	// DEBUG(__FILE__ " %d\n", __LINE__);
		kn_value_free(ret);
	inner:
		ret = kn_value_run(args[i++]);
	} while (args[i] != KN_UNDEFINED);

	return ret;
#endif
}

DECLARE_FUNCTION(assign, 2, '=') {
#ifdef KN_EXT_EQL_INTERPOLATE
	kn_value_t ret;
	struct kn_variable_t *variable;

	// if it's an identifier, special-case it where we don't evaluate it.
	if (kn_value_is_variable(args[0])) {
		variable = kn_value_as_variable(args[0]);
		ret = kn_value_run(args[1]);
	} else {
		// otherwise, evaluate the expression, convert to a string,
		// and then use that as the variable.
		variable = &kn_env_fetch(kn_value_to_string(args[0])->str, false)->value;

		ret = kn_value_run(args[1]);
	}

	if (*variable != KN_UNDEFINED) {
	// DEBUG(__FILE__ " %d\n", __LINE__);
		kn_value_free(*variable);}
	*variable = kn_value_clone(ret);

	return ret;
#else
	kn_value_t *value = &kn_value_as_variable(args[0])->value;

	if (*value != KN_UNDEFINED)
		kn_value_free(*value);

	*value = kn_value_run(args[1]);

	return kn_value_clone(*value);
#endif
}


DECLARE_FUNCTION(while, 2, 'W') {
	while (kn_value_to_boolean(args[0]))
		(void) kn_value_run(args[1]);

	return KN_NULL;
}


DECLARE_FUNCTION(if, 3, 'I') {
	kn_value_t torun = args[1 + !kn_value_to_boolean(args[0])];

	return kn_value_run(torun);
}


DECLARE_FUNCTION(get, 3, 'G') {
	const struct kn_string_t *string;
	size_t start, amnt;

	string = kn_value_to_string(args[0]);
	start = (size_t) kn_value_to_number(args[1]);
	amnt = (size_t) kn_value_to_number(args[2]);

	if (kn_string_length(string) <= start)
		return kn_value_new_string(&KN_STRING_EMPTY);

	if (kn_string_length(string) <= amnt + start)
		return kn_value_new_string(kn_string_tail(string, start));

	char *substr = xmalloc(amnt + 1);

	memcpy(substr, kn_string_deref(string), amnt);
	substr[amnt] = '\0';

	return kn_value_new_string(kn_string_emplace(substr, amnt));
}

DECLARE_FUNCTION(set, 4, 'S') {
	const struct kn_string_t *string, *substr;
	size_t start, amnt;


	string = kn_value_to_string(args[0]);
	start = (size_t) kn_value_to_number(args[1]);
	amnt = (size_t) kn_value_to_number(args[2]);
	substr = kn_value_to_string(args[3]);

	size_t string_length = kn_string_length(string);
	size_t substr_length = kn_string_length(substr);

	// if (start == 0 && substr_length == 0)
		// return kn_value_new_string(kn_string_tail(string, amnt));

	// if it's out of bounds, die.
	if (string_length < start)
		die("index '%zu' out of bounds (length=%zu)", start, string_length);

	if (string_length < start + amnt)
		amnt = string_length - start;

	size_t length = string_length - amnt + substr_length;
	char *str = xmalloc(length + 1);
	str[length] = '\0';

	char *ptr = str;

	memcpy(ptr, kn_string_deref(string), start);
	ptr += start;
	memcpy(ptr, kn_string_deref(substr), substr_length);
	ptr += substr_length;
	memcpy(ptr, kn_string_deref(string) + start + amnt, string_length - amnt);

	return kn_value_new_string(kn_string_emplace(str, length));
}
