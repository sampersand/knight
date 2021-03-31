#include "function.h" /* prototypes */
#include "knight.h"   /* kn_run */
#include "env.h"      /* kn_env_fetch, kn_variable, kn_variable_run,
                         kn_variable_assign */
#include "shared.h"   /* die, assert_reckless, xmalloc, xrealloc */
#include "string.h"   /* kn_string, kn_string_new, kn_string_alloc,
                         kn_string_free, kn_string_empty, kn_string_deref,
                         kn_string_length, kn_string_clone_static */
#include "value.h"    /* kn_value, kn_number, KN_TRUE, KN_FALSE, KN_NULL,
                         KN_UNDEFINED, kn_value_new_number, kn_value_new_string,
                         kn_value_new_boolean, kn_value_clone, kn_value_free,
                         kn_value_dump, kn_value_is_number, kn_value_is_boolean,
                         kn_value_is_string, kn_value_is_variable,
                         kn_value_as_number, kn_value_as_string,
                         kn_value_as_variable, kn_value_to_boolean,
                         kn_value_to_number, kn_value_to_string, kn_value_run */

#include <string.h>  /* memcpy, strcmp, strndup */
#include <assert.h>  /* assert */
#include <stdlib.h>  /* rand, srand, free, exit, size_t, NULL */
#include <stdbool.h> /* bool, false */
#include <stdio.h>   /* fflush, fputs, putc, puts, feof, ferror, perror, getline,
                        clearerr, stdout, stdin, popen, fread, pclose, FILE */
#include <time.h>    /* time */

void kn_function_startup(void) {
	// all we have to do on startup is seed the random number.
	srand(time(NULL));
}

KN_FUNCTION_DECLARE(prompt, 0, 'P') {
	(void) args;

	size_t cap = 0;
	ssize_t len; // todo: remove the ssize_t
	char *line = NULL;

	// TODO: use fgets instead

	// try to read a line from stdin.
	if ((len = getline(&line, &cap, stdin)) == -1) {
		assert(line != NULL);
		free(line);

#ifndef KN_RECKLESS
		// if we're not at eof, abort.
		if (!feof(stdin))
			perror("unable to read line");
#endif /* !KN_RECKLESS */

		return kn_value_new_string(&kn_string_empty);
	}

	assert(0 <= len);
	assert(line != NULL);

	char *ret = strndup(line, len);
	free(line);

	return kn_value_new_string(kn_string_new(ret, len));
}


KN_FUNCTION_DECLARE(random, 0, 'R') {
	(void) args;

	return kn_value_new_number((kn_number) rand());
}

KN_FUNCTION_DECLARE(eval, 1, 'E') {
	struct kn_string *string = kn_value_to_string(args[0]);
	kn_value ret = kn_run(kn_string_deref(string));

	kn_string_free(string);

	return ret;
}

KN_FUNCTION_DECLARE(block, 1, 'B') {
	return kn_value_clone(args[0]);
}

KN_FUNCTION_DECLARE(call, 1, 'C') {
	kn_value ran = kn_value_run(args[0]);
	kn_value result = kn_value_run(ran);

	kn_value_free(ran);

	return result;
}

KN_FUNCTION_DECLARE(system, 1, '`') {
	struct kn_string *command = kn_value_to_string(args[0]);
	const char *str = kn_string_deref(command);
	FILE *stream = popen(str, "r");

#ifndef KN_RECKLESS
	if (stream == NULL)
		die("unable to execute command '%s'.", str);
#endif /* !KN_RECKLESS */

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
#endif /* !KN_RECKLESS */

	result = xrealloc(result, len + 1);
	result[len] = '\0';

#ifndef KN_RECKLESS
	// Abort if we cant close stream.
	if (pclose(stream) == -1)
		die("unable to close command stream.");
#endif /* !KN_RECKLESS */

	return kn_value_new_string(kn_string_new(result, len));
}

KN_FUNCTION_DECLARE(quit, 1, 'Q') {
	exit((int) kn_value_to_number(args[0]));
}

KN_FUNCTION_DECLARE(not, 1, '!') {
	return kn_value_new_boolean(!kn_value_to_boolean(args[0]));
}

KN_FUNCTION_DECLARE(length, 1, 'L') {
	struct kn_string *string = kn_value_to_string(args[0]);
	size_t length = kn_string_length(string);

	kn_string_free(string);

	return kn_value_new_number((kn_number) length);
}

KN_FUNCTION_DECLARE(dump, 1, 'D') {
	kn_value ret = kn_value_run(args[0]);

	kn_value_dump(ret);

	putc('\n', stdout);

	return ret;
}

KN_FUNCTION_DECLARE(output, 1, 'O') {
	struct kn_string *string = kn_value_to_string(args[0]);
	size_t length = kn_string_length(string);
	char *str = kn_string_deref(string);
	char *penult;

#ifndef KN_RECKLESS
	clearerr(stdout);
#endif /* !KN_RECKLESS */

	if (length == 0) {
		putc('\n', stdout);
	} else if ('\\' == *(penult = &str[length - 1])) {
		*penult = '\0'; // replace the trailing `\`, so it wont be printed
		fputs(str, stdout);
		*penult = '\\'; // ...and then restore it.
		fflush(stdout);
	} else {
		puts(str);
	}

#ifndef KN_RECKLESS
	if (ferror(stdout))
		die("unable to write to stdout");
#endif /* !KN_RECKLESS */

	kn_string_free(string);

	return KN_NULL;
}

#ifdef KN_EXT_VALUE
KN_FUNCTION_DECLARE(value, 1, 'V') {
	struct kn_string *string = kn_value_to_string(args[0]);
	struct kn_variable *variable = kn_env_fetch(kn_string_deref(string), false);

	kn_string_free(string);
	return kn_variable_run(variable);
}
#endif /* KN_EXT_VALUE */

#ifdef KN_EXT_NEGATE
KN_FUNCTION_DECLARE(negate, 1, '~') {
	return kn_value_new_number(-kn_value_to_number(args[0]));
}
#endif /* KN_EXT_NEGATE */

static kn_value add_string(struct kn_string *lhs, struct kn_string *rhs) {
	size_t lhslen, rhslen;

	// return early if either
	if ((lhslen = kn_string_length(lhs)) == 0) {
		assert(lhs == &kn_string_empty);

		return kn_value_new_string(kn_string_clone_static(rhs));
	}

	if ((rhslen = kn_string_length(rhs)) == 0) {
		assert(rhs == &kn_string_empty);

		return kn_value_new_string(lhs);
	}

	size_t length = lhslen + rhslen;
	struct kn_string *string = kn_string_alloc(length);
	char *str = kn_string_deref(string);

	memcpy(str, kn_string_deref(lhs), lhslen);
	memcpy(str + lhslen, kn_string_deref(rhs), rhslen);
	str[length] = '\0';

	kn_string_free(lhs);
	kn_string_free(rhs);

	return kn_value_new_string(string);
}

KN_FUNCTION_DECLARE(add, 2, '+') {
	kn_value lhs = kn_value_run(args[0]);

	// If lhs is a string, convert both to a string and concatenate.
	if (kn_value_is_string(lhs))
		return add_string(kn_value_as_string(lhs), kn_value_to_string(args[1]));

	assert_reckless(kn_value_is_number(lhs));

	kn_number lhs_num = kn_value_as_number(lhs);
	kn_number rhs_num = kn_value_to_number(args[1]);

	return kn_value_new_number(lhs_num + rhs_num);
}

KN_FUNCTION_DECLARE(sub, 2, '-') {
	kn_value lhs = kn_value_run(args[0]);

	assert_reckless(kn_value_is_number(lhs));

	kn_number lhs_num = kn_value_as_number(lhs);
	kn_number rhs_num = kn_value_to_number(args[1]);

	return kn_value_new_number(lhs_num - rhs_num);
}

static kn_value mul_string(struct kn_string *lhs, size_t times) {
	size_t lhslen = kn_string_length(lhs);

	if (lhslen == 0 || times == 0) {

		// if the string is not empty, free it.
		if (lhslen != 0)
			kn_string_free(lhs);
		else
			assert(lhs == &kn_string_empty);

		return kn_value_new_string(&kn_string_empty);
	}

	// we don't have to clone it, as we were given the cloned copy.
	if (times == 1)
		return kn_value_new_string(lhs);

	size_t length = lhslen * times;
	struct kn_string *string = kn_string_alloc(length);
	char *str = kn_string_deref(string);

	for (char *ptr = str; times != 0; --times, ptr += lhslen)
		memcpy(ptr, kn_string_deref(lhs), lhslen);

	str[length] = '\0';

	kn_string_free(lhs);

	return kn_value_new_string(string);
}

KN_FUNCTION_DECLARE(mul, 2, '*') {
	kn_value lhs = kn_value_run(args[0]);

	// If lhs is a string, convert rhs to a number and multiply.
	if (kn_value_is_string(lhs)) {
		size_t amnt = (size_t) kn_value_to_number(args[1]);
		return mul_string(kn_value_as_string(lhs), amnt);
	}

	assert_reckless(kn_value_is_number(lhs));

	kn_number lhs_num = kn_value_as_number(lhs);
	kn_number rhs_num = kn_value_to_number(args[1]);

	return kn_value_new_number(lhs_num * rhs_num);
}

KN_FUNCTION_DECLARE(div, 2, '/') {
	kn_value lhs = kn_value_run(args[0]);

	assert_reckless(kn_value_is_number(lhs));

	kn_number dividend = kn_value_as_number(lhs);
	kn_number divisor = kn_value_to_number(args[1]);

#ifndef KN_RECKLESS
	if (divisor == 0)
		die("attempted to divide by zero");
#endif /* !KN_RECKLESS */

	return kn_value_new_number(dividend / divisor);
}

KN_FUNCTION_DECLARE(mod, 2, '%') {
	kn_value lhs = kn_value_run(args[0]);

	assert_reckless(kn_value_is_number(lhs));

	kn_number number = kn_value_as_number(lhs);
	kn_number base = kn_value_to_number(args[1]);

#ifndef KN_RECKLESS
	if (base == 0)
		die("attempted to modulo by zero");
#endif /* !KN_RECKLESS */

	return kn_value_new_number(number % base);
}

KN_FUNCTION_DECLARE(pow, 2, '^') {
	kn_number result;
	kn_number base = kn_value_to_number(args[0]);
	kn_number exponent = kn_value_to_number(args[1]);

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

KN_FUNCTION_DECLARE(eql, 2, '?') {
	kn_value lhs = kn_value_run(args[0]);
	kn_value rhs = kn_value_run(args[1]);
	bool eql;

	assert(lhs != KN_UNDEFINED);
	assert(rhs != KN_UNDEFINED);

	if ((eql = (lhs == rhs)))
		goto free_and_return;

	if (!(eql = (kn_value_is_string(lhs) && kn_value_is_string(rhs))))
		goto free_and_return;

	struct kn_string *lstr = kn_value_as_string(lhs);
	struct kn_string *rstr = kn_value_as_string(rhs);

	eql = kn_string_length(lstr) == kn_string_length(rstr) &&
		!strcmp(kn_string_deref(lstr), kn_string_deref(rstr));

free_and_return:

	kn_value_free(lhs);
	kn_value_free(rhs);

	return kn_value_new_boolean(eql);
}

KN_FUNCTION_DECLARE(lth, 2, '<') {
	kn_value lhs = kn_value_run(args[0]);
	bool less;

	if (kn_value_is_string(lhs)) {
		struct kn_string *lstr = kn_value_as_string(lhs);
		struct kn_string *rstr = kn_value_to_string(args[1]);

		less = strcmp(kn_string_deref(lstr), kn_string_deref(rstr)) < 0;

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

KN_FUNCTION_DECLARE(gth, 2, '>') {
	kn_value lhs = kn_value_run(args[0]);
	bool more;

	if (kn_value_is_string(lhs)) {
		struct kn_string *lstr = kn_value_as_string(lhs);
		struct kn_string *rstr = kn_value_to_string(args[1]);

		more = strcmp(kn_string_deref(lstr), kn_string_deref(rstr)) > 0;

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

KN_FUNCTION_DECLARE(and, 2, '&') {
	kn_value lhs = kn_value_run(args[0]);

	// return the lhs if its falsey.
	if (!kn_value_to_boolean(lhs))
		return lhs;

	kn_value_free(lhs);
	return kn_value_run(args[1]);
}

KN_FUNCTION_DECLARE(or, 2, '|') {
	kn_value lhs = kn_value_run(args[0]);

	// return the lhs if its truthy.
	if (kn_value_to_boolean(lhs))
		return lhs;

	kn_value_free(lhs);
	return kn_value_run(args[1]);
}

KN_FUNCTION_DECLARE(then, 2, ';') {
	kn_value_free(kn_value_run(args[0]));

	return kn_value_run(args[1]);
}

KN_FUNCTION_DECLARE(assign, 2, '=') {
	struct kn_variable *variable;
	kn_value ret;

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
		variable =
			kn_env_fetch(kn_string_deref(kn_value_to_string(args[0])), false);
		ret = kn_value_run(args[1]);
	}
#endif /* KN_EXT_EQL_INTERPOLATE */

	kn_variable_assign(variable, kn_value_clone(ret));

	return ret;
}

KN_FUNCTION_DECLARE(while, 2, 'W') {
	while (kn_value_to_boolean(args[0]))
		kn_value_free(kn_value_run(args[1]));

	return KN_NULL;
}

KN_FUNCTION_DECLARE(if, 3, 'I') {
	bool idx = kn_value_to_boolean(args[0]);

	return kn_value_run(args[1 + !idx]);
}

KN_FUNCTION_DECLARE(get, 3, 'G') {
	struct kn_string *string, *substring;
	size_t start, length, stringlen;

	string = kn_value_to_string(args[0]);
	start = (size_t) kn_value_to_number(args[1]);
	length = (size_t) kn_value_to_number(args[2]);

	stringlen = kn_string_length(string);

	// if we're getting past the end of the array, simply return the
	// empty string.
	if (stringlen <= start) {
		substring = &kn_string_empty;
		goto free_and_return;
	}

	// if the total length is too much, simply wrap around to the end.
	if (stringlen <= start + length)
		length = stringlen - start;

	substring = kn_string_alloc(length);
	char *substr = kn_string_deref(substring);

	memcpy(substr, kn_string_deref(string) + start, length);
	substr[length] = '\0';

free_and_return:

	kn_string_free(string);

	return kn_value_new_string(substring);
}

KN_FUNCTION_DECLARE(substitute, 4, 'S') {
	struct kn_string *string, *substring, *result;
	size_t start, amnt, length, stringlength, substringlength;

	string = kn_value_to_string(args[0]);
	start = (size_t) kn_value_to_number(args[1]);
	amnt = (size_t) kn_value_to_number(args[2]);
	substring = kn_value_to_string(args[3]);

	stringlength = kn_string_length(string);

#ifndef KN_RECKLESS
	// if it's out of bounds, die.
	if (stringlength < start)
		die("index '%zu' out of bounds (length=%zu)", start, stringlength);
#endif /* !KN_RECKLESS */

	if (stringlength <= start + amnt)
		amnt = stringlength - start;

	substringlength = kn_string_length(substring);

	length = stringlength - amnt + substringlength;
	result = kn_string_alloc(length);
	char *ptr = kn_string_deref(result);
	ptr[length] = '\0';

	memcpy(ptr, kn_string_deref(string), start);
	ptr += start;
	memcpy(ptr, kn_string_deref(substring), substringlength);
	ptr += substringlength;
	memcpy(ptr, kn_string_deref(string) + start + amnt, stringlength - amnt);

	kn_string_free(string);
	kn_string_free(substring);

	return kn_value_new_string(result);
}

