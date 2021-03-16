#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <time.h>

#include "shared.h"
#include "function.h"
#include "knight.h"
#include "value.h"
#include "env.h"

void kn_function_startup(void) {
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

DECLARE_FUNCTION(prompt, 0, 'P') {
	(void) args;

	size_t cap = 0;
	ssize_t len;
	char *line = NULL;

	// try to read a line from stdin.
	if ((len = getline(&line, &cap, stdin)) == -1) {
		assert(line != NULL);
		free(line);

#ifndef KN_RECKLESS
		// if we're not at eof, aort.
		if (!feof(stdin)) 
			perror("unable to read line");
#endif /* KN_RECKLESS */

		return kn_value_new_string(&KN_STRING_EMPTY);
	}

	assert(0 <= len);
	assert(line != NULL);

	char *ret = strndup(line, len);
	free(line);

	return kn_value_new_string(kn_string_new(ret, len));
}


DECLARE_FUNCTION(random, 0, 'R') {
	(void) args;

	return kn_value_new_number((kn_number_t) rand());
}

#ifdef KN_EXT_VALUE
DECLARE_FUNCTION(value, 1, 'V') {
	struct kn_string_t *string = kn_value_to_string(args[0]);
	struct kn_variable_t *variable = kn_env_fetch(string->str, false);

	kn_string_free(string);

#ifndef KN_RECKLESS
	if (variable->value == KN_UNDEFINED)
		die("undefined variable '%s'", variable->str);
#endif /* KN_RECKLESS */

	return kn_value_clone(variable->value);
}
#endif /* KN_EXT_VALUE */

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

DECLARE_FUNCTION(system, 1, '`') {
	struct kn_string_t *command = kn_value_to_string(args[0]);
	FILE *stream = popen(command->str, "r");

#ifndef KN_RECKLESS
	if (stream == NULL)
		die("unable to execute command '%s'.", command->str);
#endif /* KN_RECKLESS */

	kn_string_free(command);

	size_t tmp;
	size_t cap = 2048;
	size_t len = 0;
	char *result = xmalloc(cap);

	// try to read the entire stream's stdout to `result`.
	while (0 != (tmp = fread(result + len, 1, cap - len, stream))) {
		len += tmp;

		if (len == cap) {
			cap *= 2;
			result = xrealloc(result, cap);
		}
	}

#ifndef KN_RECKLESS
	// Abort if `stream` had an error.
	if (ferror(stream))
		die("unable to read command stream");
#endif /* KN_RECKLESS */

	result = xrealloc(result, len + 1);
	result[len] = '\0';

#ifndef KN_RECKLESS
	// Abort if we cant close stream.
	if (pclose(stream) == -1)
		die("unable to close command stream.");
#endif /* KN_RECKLESS */

	return kn_value_new_string(kn_string_new(result, len));
}

DECLARE_FUNCTION(quit, 1, 'Q') {
	exit((int) kn_value_to_number(args[0]));
}

DECLARE_FUNCTION(not, 1, '!') {
	return kn_value_new_boolean(!kn_value_to_boolean(args[0]));
}

DECLARE_FUNCTION(length, 1 ,'L') {
	struct kn_string_t *string = kn_value_to_string(args[0]);
	size_t length = string->length;

	kn_string_free(string);

	return kn_value_new_number((kn_number_t) length);
}

DECLARE_FUNCTION(dump, 1 ,'D') {
	kn_value_t ret = kn_value_run(args[0]);

	kn_value_dump(ret);

	return ret;
}

DECLARE_FUNCTION(output, 1, 'O') {
	struct kn_string_t *string = kn_value_to_string(args[0]);

	// right here we're casting away the const.
	// this is because we might need to replace the penult character
	// with a `\0` if it's a backslash to prevent the printing of a
	// newline. however, we replace it later, so it's ok. (assuming the
	// input string doesn't have a `\\` in it, but no interned strings do.)
	char *penult;

#ifndef KN_RECKLESS
	clearerr(stdout);
#endif /* KN_RECKLESS */

	if (string->length == 0) {
		putc('\n', stdout);
	} else if ('\\' == *(penult = (char *) &string->str[string->length - 1])) {
		*penult = '\0'; // replace the trailing `\`...
		fputs(string->str, stdout);
		*penult = '\\'; // ...and then restore it.
		fflush(stdout);
	} else {
		puts(string->str);
	}

#ifndef KN_RECKLESS
	if (ferror(stdout))
		die("unable to write to stdout");
#endif /* KN_RECKLESS */

	kn_string_free(string);

	return KN_NULL;
}

static kn_value_t add_string(struct kn_string_t *lhs, struct kn_string_t *rhs) {
	// note that all length-zero strings are always interned, so no need to free
	if (lhs->length == 0) {
		assert(kn_string_is_interned(lhs));

		return kn_value_new_string(rhs);
	}

	if (rhs->length == 0) {
		assert(kn_string_is_interned(rhs));

		return kn_value_new_string(lhs);
	}

	size_t length = lhs->length + rhs->length;
	char *str = xmalloc(length + 1);

	memcpy(str, lhs->str, lhs->length);
	memcpy(str + lhs->length, rhs->str, rhs->length);

	str[length] = '\0';

	kn_string_free(lhs);
	kn_string_free(rhs);

	return kn_value_new_string(kn_string_new(str, length));
}

DECLARE_FUNCTION(add, 2, '+') {
	kn_value_t lhs = kn_value_run(args[0]);

	// If lhs is a string, convert both to a string and concat
	if (kn_value_is_string(lhs))
		return add_string(kn_value_as_string(lhs), kn_value_to_string(args[1]));

	assert_reckless(kn_value_is_number(lhs));

	kn_number_t lhs_num = kn_value_as_number(lhs);
	kn_number_t rhs_num = kn_value_to_number(args[1]);

	return kn_value_new_number(lhs_num + rhs_num);
}

DECLARE_FUNCTION(sub, 2, '-') {
	kn_value_t lhs = kn_value_run(args[0]);

	assert_reckless(kn_value_is_number(lhs));

	kn_number_t lhs_num = kn_value_as_number(lhs);
	kn_number_t rhs_num = kn_value_to_number(args[1]);

	return kn_value_new_number(lhs_num - rhs_num);
}

static kn_value_t mul_string(struct kn_string_t *lhs, size_t times) {
	if (lhs->length == 0 || times == 0) {

		// if the string is not empty, free it.
		if (lhs->length != 0)
			kn_string_free(lhs); 
		else // otherwise, the string should e interned.
			assert(kn_string_is_interned(lhs));

		return kn_value_new_string(&KN_STRING_EMPTY);
	}

	// we don't have to clone it, as we were given the cloned copy.
	if (times == 1)
		return kn_value_new_string(lhs);

	size_t length = lhs->length * times;
	char *str = xmalloc(length + 1);

	for (char *ptr = str; times != 0; --times, ptr += lhs->length)
		memcpy(ptr, lhs->str, lhs->length);

	str[length] = '\0';

	return kn_value_new_string(kn_string_new(str, length));
}

DECLARE_FUNCTION(mul, 2, '*') {
	kn_value_t lhs = kn_value_run(args[0]);

	// If lhs is a string, convert rhs to a number and multiply.
	if (kn_value_is_string(lhs)) {
		return mul_string(
			kn_value_as_string(lhs),
			(size_t) kn_value_to_number(args[1]));
	}
	
	assert_reckless(kn_value_is_number(lhs));

	kn_number_t lhs_num = kn_value_as_number(lhs);
	kn_number_t rhs_num = kn_value_to_number(args[1]);

	return kn_value_new_number(lhs_num * rhs_num);
}

DECLARE_FUNCTION(div, 2, '/') {
	kn_value_t lhs = kn_value_run(args[0]);

	assert_reckless(kn_value_is_number(lhs));

	kn_number_t dividend = kn_value_as_number(lhs);
	kn_number_t divisor = kn_value_to_number(args[1]);

#ifndef KN_RECKLESS
	if (divisor == 0)
		die("attempted to divide by zero");
#endif /* KN_RECKLESS */

	return kn_value_new_number(dividend / divisor);
}

DECLARE_FUNCTION(mod, 2, '%') {
	kn_value_t lhs = kn_value_run(args[0]);

	assert_reckless(kn_value_is_number(lhs));

	kn_number_t number = kn_value_as_number(lhs);
	kn_number_t base = kn_value_to_number(args[1]);

#ifndef KN_RECKLESS
	if (base == 0)
		die("attempted to modulo by zero");
#endif /* KN_RECKLESS */

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
	else {
		for (result = 1; exponent > 0; --exponent)
			result *= base;
	}

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
		assert_reckless(kn_value_is_boolean(lhs));

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
		assert_reckless(kn_value_is_boolean(lhs));

		more = !kn_value_to_boolean(args[1]) && lhs == KN_TRUE;
	}

	return kn_value_new_boolean(more);
}

DECLARE_FUNCTION(and, 2, '&') {
	kn_value_t lhs = kn_value_run(args[0]);

	// return the lhs if its falsey.
	if (!kn_value_to_boolean(lhs))
		return lhs;

	kn_value_free(lhs);
	return kn_value_run(args[1]);
}

DECLARE_FUNCTION(or, 2, '|') {
	kn_value_t lhs = kn_value_run(args[0]);

	// return the lhs if its truthy.
	if (kn_value_to_boolean(lhs))
		return lhs;

	kn_value_free(lhs);
	return kn_value_run(args[1]);
}

DECLARE_FUNCTION(then, 2, ';') {
#ifndef KN_DYNMAIC_ARGC
	kn_value_free(kn_value_run(args[0]));

	return kn_value_run(args[1]);
#else
	kn_value_t ret;
	unsigned i = 0;

	goto inner;

	do {
		kn_value_free(ret);
	inner:
		ret = kn_value_run(args[i]);
	} while (args[++i] != KN_UNDEFINED);

	return ret;
#endif
}

DECLARE_FUNCTION(assign, 2, '=') {
	struct kn_variable_t *variable;
	kn_value_t ret;

#ifdef KN_EXT_EQL_INTERPOLATE
	// if it's an identifier, special-case it where we don't evaluate it.
	if (kn_value_is_variable(args[0])) {
#endif /* KN_EXT_EQL_INTERPOLATE */

	variable = kn_value_as_variable(args[0]);
	ret = kn_value_run(args[1]);

#ifdef KN_EXT_EQL_INTERPOLATE
	} else {
		// otherwise, evaluate the expression, convert to a string,
		// and then use that as the variable.
		variable = kn_env_fetch(kn_value_to_string(args[0])->str, false);
		ret = kn_value_run(args[1]);
	}
#endif /* KN_EXT_EQL_INTERPOLATE */

	// since it's new variables are undefined, we shouldn't free its old value
	// if we haven't assigned to it yet.
	if (variable->value != KN_UNDEFINED)
		kn_value_free(variable->value);

	variable->value = ret;

	return kn_value_clone(ret);
}


DECLARE_FUNCTION(while, 2, 'W') {
	while (kn_value_to_boolean(args[0]))
		kn_value_free(kn_value_run(args[1]));

	return KN_NULL;
}

DECLARE_FUNCTION(if, 3, 'I') {
	bool idx = kn_value_to_boolean(args[0]);

	return kn_value_run(args[1 + !idx]);
}

DECLARE_FUNCTION(get, 3, 'G') {
	struct kn_string_t *substring;
	struct kn_string_t *string = kn_value_to_string(args[0]);
	size_t start = (size_t) kn_value_to_number(args[1]);
	size_t length = (size_t) kn_value_to_number(args[2]);

	// if we're getting past the end of the array, simply return the
	// empty string.
	if (string->length <= start) {
		substring = &KN_STRING_EMPTY;
		goto free_and_return;
	}

	// if the total length is too much, simply wrap around to the end.
	if (string->length <= start + length)
		length = string->length - start;

	char *substr = xmalloc(length + 1);

	memcpy(substr, string->str + start, length);
	substr[length] = '\0';

	substring = kn_string_new(substr, length);

free_and_return:
	kn_string_free(string);

	return kn_value_new_string(substring);
}

DECLARE_FUNCTION(set, 4, 'S') {
	struct kn_string_t *string = kn_value_to_string(args[0]);
	size_t start = (size_t) kn_value_to_number(args[1]);
	size_t amnt = (size_t) kn_value_to_number(args[2]);
	struct kn_string_t *substr = kn_value_to_string(args[3]);

#ifndef KN_RECKLESS
	// if it's out of bounds, die.
	if (string->length < start)
		die("index '%zu' out of bounds (length=%zu)", start,
			string->length);
#endif /* KN_RECKLESS */

	if (string->length <= start + amnt)
		amnt = string->length - start;

	size_t length = string->length - amnt + substr->length;
	char *str = xmalloc(length + 1);
	str[length] = '\0';

	char *ptr = str;

	memcpy(ptr, string->str, start);
	ptr += start;
	memcpy(ptr, substr->str, substr->length);
	ptr += substr->length;
	memcpy(ptr, string->str + start + amnt, string->length - amnt);

	kn_string_free(string);
	kn_string_free(substr);

	return kn_value_new_string(kn_string_new(str, length));
}
