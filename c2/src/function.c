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

void kn_function_initialize(void) {
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
			return kn_value_new_string(KN_STRING_EMPTY);
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
	return kn_value_clone(*kn_env_fetch(kn_string_deref(kn_value_to_string(args[0])), false));
}
#endif

DECLARE_FUNCTION(eval, 1, 'E') {
	return kn_run(kn_string_deref(kn_value_to_string(args[0])));
}

DECLARE_FUNCTION(block, 1, 'B') {
	return kn_value_clone(args[0]);
}

DECLARE_FUNCTION(call, 1, 'C') {
	kn_value_t arg0 = kn_value_run(args[0]);
	kn_value_t ret = kn_value_run(arg0);

	// DEBUG(__FILE__ " %d\n", __LINE__);
	kn_value_free(arg0);
	return ret;
}

#ifndef KN_EMBEDDED
DECLARE_FUNCTION(system, 1, '`') {
	const struct kn_string_t *command = kn_value_to_string(args[0]);

	FILE *stream = popen(kn_string_deref(command), "r");

	if (stream == NULL)
		die("unable to execute command '%s'.", kn_string_deref(command));

	size_t cap = 2048;
	size_t len = 0;
	size_t tmp;
	char *result = xmalloc(cap);

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

	return kn_value_new_string(kn_string_new(result));
}
#endif /* KN_EMBEDDED */

#ifndef KN_EMBEDDED
DECLARE_FUNCTION(quit, 1, 'Q') {
	kn_number_t code = kn_value_to_number(args[0]);

#ifndef RECKLESS
	assert((kn_number_t) (int) code == code);
#endif

	exit((int) code);
}
#endif /* KN_EMBEDDED */

DECLARE_FUNCTION(not, 1, '!') {
	return kn_value_new_boolean(!kn_value_to_boolean(args[0]));
}

DECLARE_FUNCTION(length, 1 ,'L') {
	kn_number_t length = (kn_number_t) kn_string_length(kn_value_to_string(args[0]));

	return kn_value_new_number(length);
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
	const struct kn_string_t *string = kn_value_to_string(args[0]);
	const char *str = kn_string_deref(string);
	size_t length = kn_string_length(string);

	// right here we're casting away the const.
	// this is because we might need to replace the penult character
	// with a `\0` if it's a backslash to prevent the printing of a
	// newline. however, we replace it on the next line, so it's ok.
	char *penult;

	if (length != 0 && *(penult = (char *) &str[length - 1]) == '\\') {
		*penult = '\0'; // replace the trailing `\`
		printf("%s", str);
		*penult = '\\'; // and then restore it.
	} else {
		printf("%s\n", str);
	}

	return KN_NULL;
}
#endif /* KN_EMBEDDED */

static kn_value_t kn_fn_add_string(
	const struct kn_string_t *lhs,
	const struct kn_string_t *rhs
) {
	size_t lhslength = kn_string_length(lhs);
	size_t rhslength = kn_string_length(rhs);

	if (lhslength == 0) return kn_value_new_string(rhs);
	if (rhslength == 0) return kn_value_new_string(lhs);

	size_t length = lhslength + rhslength;
	char *str = xmalloc(length + 1);

	memcpy(str, kn_string_deref(lhs), lhslength);
	memcpy(str + lhslength, kn_string_deref(rhs), rhslength);
	str[length] = '\0';

	return kn_value_new_string(kn_string_emplace(str, length));
}

DECLARE_FUNCTION(add, 2, '+') {
	kn_value_t lhs = kn_value_run(args[0]);

	// If lhs is a string, convert both to a string and concat
	if (kn_value_is_string(lhs)) {
		return kn_fn_add_string(
			kn_value_as_string(lhs),
			kn_value_to_string(args[1]));
	}

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

static kn_value_t kn_fn_mul_string(const struct kn_string_t *lhs, size_t amnt) {
	// die("<todo: verify mul string>");
	// if we have an empty string, return early.
	if (kn_string_length(lhs) == 0 || amnt == 1)
		// TODO: do we clone this or not.
		return kn_value_new_string(kn_string_clone(lhs));

	if (amnt == 0)
		return kn_value_new_string(KN_STRING_EMPTY);

	size_t lhslength = kn_string_length(lhs);
	size_t length = lhslength * amnt;
	char *str = xmalloc(length + 1);
	char *ptr = str;

	for (; amnt != 0; --amnt, ptr += lhslength)
		memcpy(ptr, kn_string_deref(lhs), lhslength);

	*ptr = '\0';

	return kn_value_new_string(kn_string_emplace(str, length));
}

DECLARE_FUNCTION(mul, 2, '*') {
	kn_value_t lhs = kn_value_run(args[0]);

	// If lhs is a string, convert both to a string and concat
	if (kn_value_is_string(lhs)) {
		return kn_fn_mul_string(
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

#ifndef RECKLESS
	if (divisor == 0)
		die("attempted to divide by zero");
#endif

	return kn_value_new_number(dividend / divisor);
}

DECLARE_FUNCTION(mod, 2, '%') {
	kn_value_t lhs = kn_value_run(args[0]);

	assert_reckless(kn_value_is_number(lhs));

	kn_number_t number = kn_value_as_number(lhs);
	kn_number_t base = kn_value_to_number(args[1]);

#ifndef RECKLESS
	if (base == 0)
		die("attempted to modulo by zero");
#endif

	return kn_value_new_number(number % base);
}

DECLARE_FUNCTION(pow, 2, '^') {
	kn_value_t lhs = kn_value_run(args[0]);

	assert_reckless(kn_value_is_number(lhs));

	kn_number_t result = 1;
	kn_number_t base = kn_value_as_number(lhs);
	kn_number_t exponent = kn_value_to_number(args[1]);

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

		for (; exponent > 0; --exponent)
			result *= base;
	}

	return kn_value_new_number(result);
}

DECLARE_FUNCTION(eql, 2, '?') {
	kn_value_t lhs = kn_value_run(args[0]);
	kn_value_t rhs = kn_value_run(args[1]);

	bool eql = kn_value_eql(lhs, rhs);

	// DEBUG(__FILE__ " %d\n", __LINE__);
	kn_value_free(lhs);
	// DEBUG(__FILE__ " %d\n", __LINE__);
	kn_value_free(rhs);

	return kn_value_new_boolean(eql);
}

DECLARE_FUNCTION(lth, 2, '<') {
	kn_value_t lhs = kn_value_run(args[0]);
	bool less;

	if (kn_value_is_string(lhs)) {
		const struct kn_string_t *lstr = kn_value_as_string(lhs);
		const struct kn_string_t *rstr = kn_value_to_string(args[1]);

		less =/* lstr->length < rstr->length ||*/ strcmp(kn_string_deref(lstr), kn_string_deref(rstr)) < 0;
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
		const struct kn_string_t *lstr = kn_value_as_string(lhs);
		const struct kn_string_t *rstr = kn_value_to_string(args[1]);

		more =/* lstr->length > rstr->length ||*/ strcmp(kn_string_deref(lstr), kn_string_deref(rstr)) > 0;
	} else if (kn_value_is_number(lhs)) {
		more = kn_value_as_number(lhs) > kn_value_to_number(args[1]);
	} else {
		assert_reckless(kn_value_is_boolean(lhs));

		more = !kn_value_to_boolean(args[1]) && lhs == KN_TRUE;
	}

	return kn_value_new_boolean(more);
}

DECLARE_FUNCTION(and, 2, '&') {
	kn_value_t ret = kn_value_run(args[0]);

	// execute the RHS if the LHS is true.
	if (!kn_value_to_boolean(ret))
		return ret;

	// DEBUG(__FILE__ " %d\n", __LINE__);
	kn_value_free(ret);
	return kn_value_run(args[1]);
}

DECLARE_FUNCTION(or, 2, '|') {
	kn_value_t ret = kn_value_run(args[0]);

	// execute the RHS if the LHS is true.
	if (kn_value_to_boolean(ret))
		return ret;

	// DEBUG(__FILE__ " %d\n", __LINE__);
	kn_value_free(ret);
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
	kn_value_t ret;
	kn_value_t *ptr;

	// if it's an identifier, special-case it where we don't evaluate it.
	if (kn_value_is_identifier(args[0])) {
		ptr = kn_value_as_identifier(args[0]);
		ret = kn_value_run(args[1]);
	} else {
		// otherwise, evaluate the expression, convert to a string,
		// and then use that as the identifier.
		ptr = kn_env_fetch(kn_string_deref(kn_value_to_string(args[0])), false);

		ret = kn_value_run(args[1]);
	}

	if (*ptr != KN_UNDEFINED) {
	// DEBUG(__FILE__ " %d\n", __LINE__);
		kn_value_free(*ptr);}
	*ptr = kn_value_clone(ret);

	return ret;
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
		return kn_value_new_string(KN_STRING_EMPTY);

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
