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

// this is a workaround and i only use one argument.
#define DECLARE_FUNCTION(_func, _arity, _name, ...) \
	static kn_value_t fn_##_func##_function(const kn_value_t *args) { \
		assert(args != NULL); \
		__VA_ARGS__ \
	} \
	struct kn_function_t kn_fn_##_func = (struct kn_function_t) { \
		.ptr = fn_##_func##_function, \
		.arity = _arity, \
		.name = _name \
	}


DECLARE_FUNCTION(prompt, 0, 'P', {
	(void) args;

	size_t cap = 0;
	ssize_t slen;
	char *line = NULL;

	// try to read a line from stdin.
	if ((slen = getline(&line, &cap, stdin)) == -1) {
		// if we're at eof, return an emtpy string. otherwise, abort.
		if (feof(stdin)) {
			return kn_value_new_string(&KN_STRING_EMPTY);
		} else {
			perror("unable to read line");
		}
	}

	size_t len = (size_t) slen;

	char *ret = xmalloc(len + 1);
	memcpy(ret, line, len);
	ret[len] = '\0';

	return kn_value_new_string(kn_string_new(ret));
});


DECLARE_FUNCTION(rand, 0, 'R', {
	(void) args;

	return kn_value_new_number((kn_number_t) rand());
});

DECLARE_FUNCTION(eval, 1, 'E', {
	const struct kn_string_t *arg0 = kn_value_to_string(args[0]);
	kn_value_t ret = kn_run(arg0->str);
	kn_string_free(arg0);
	return ret;
});

DECLARE_FUNCTION(block, 1, 'B', {
	return kn_value_clone(args[0]);
});

DECLARE_FUNCTION(call, 1, 'C', {
	kn_value_t arg0 = kn_value_run(args[0]);
	kn_value_t ret = kn_value_run(arg0);

	kn_value_free(arg0);
	return ret;
});

DECLARE_FUNCTION(system, 1, '`', {
	const struct kn_string_t *command = kn_value_to_string(args[0]);

	FILE *stream = popen(command->str, "r");

	if (stream == NULL) {
		die("unable to execute command '%s'.", command->str);
	}

	kn_string_free(command);


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

	return kn_value_new_string(kn_string_new(result));
});

DECLARE_FUNCTION(quit, 1, 'Q', {
	// no need to free the `ecode` value because we're exiting anyways.
	exit((int) kn_value_to_number(args[0]));
});

DECLARE_FUNCTION(not, 1, '!', {
	return kn_value_new_boolean(!kn_value_to_boolean(args[0]));
});

DECLARE_FUNCTION(length, 1 ,'L', {
	const struct kn_string_t *string = kn_value_to_string(args[0]);

	kn_value_t ret =
		kn_value_new_number((kn_number_t) strlen(string->str));

	kn_string_free(string);

	return ret;
});

DECLARE_FUNCTION(dump, 1 ,'D', {
	kn_value_t ret = kn_value_run(args[0]);
	kn_value_dump(ret);
	return ret;
});

DECLARE_FUNCTION(output, 1, 'O', {
	const struct kn_string_t *string = kn_value_to_string(args[0]);

	// right here we're casting away the const.
	// this is because we might need to replace the penult character
	// with a `\0` if it's a backslash to prevent the printing of a
	// newline. however, we replace it on the next line, so it's ok.
	char *str = (char *) string->str;
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

	kn_string_free(string);

	return KN_NULL;
});

static kn_value_t kn_fn_add_string(
	const struct kn_string_t *lhs,
	const struct kn_string_t *rhs
) {
	size_t lhslen = strlen(lhs->str);
	size_t rhslen = strlen(rhs->str);

	if (lhslen == 0) {
		kn_string_free(lhs);
		return kn_value_new_string(rhs);
	} else if (rhslen == 0) {
		kn_string_free(rhs);
		return kn_value_new_string(kn_string_clone(lhs));
	}

	char *ret = xmalloc(lhslen + rhslen + 1);

	memcpy(ret, lhs->str, lhslen);
	memcpy(ret + lhslen, rhs->str, rhslen);

	ret[lhslen + rhslen] = '\0';

	kn_string_free(lhs);
	kn_string_free(rhs);

	return kn_value_new_string(kn_string_new(ret));
}

DECLARE_FUNCTION(add, 2, '+', {
	kn_value_t lhs = kn_value_run(args[0]);

	// If lhs is a string, convert both to a string and concat
	if (kn_value_is_string(lhs)) {
		return kn_fn_add_string(
			kn_value_as_string(lhs),
			kn_value_to_string(args[1]));
	} else {
		kn_number_t number = kn_value_to_number(lhs);
		kn_value_free(lhs);

		return kn_value_new_number(
			number + kn_value_to_number(args[1]));
	}
});

DECLARE_FUNCTION(sub, 2, '-', {
	return kn_value_new_number(
		kn_value_to_number(args[0]) - kn_value_to_number(args[1]));
});

static kn_value_t kn_fn_mul_string(const struct kn_string_t *lhs, size_t amnt) {
	size_t lhslen = strlen(lhs->str);

	// if we have an empty string, return early.
	if (lhslen == 0 || amnt == 1) {
		return kn_value_new_string(kn_string_clone(lhs));
	} else if (amnt == 0) {
		kn_string_free(lhs);
		return kn_value_new_string(&KN_STRING_EMPTY);
	}

	char *string = xmalloc(1 + lhslen * amnt);
	char *ptr = string;

	for (; amnt != 0; --amnt) {
		memcpy(ptr, lhs->str, lhslen);
		ptr += lhslen;
	}

	*ptr = '\0';

	kn_string_free(lhs);

	return kn_value_new_string(kn_string_new(string));
}

DECLARE_FUNCTION(mul, 2, '*', {
	kn_value_t lhs = kn_value_run(args[0]);

	// If lhs is a string, convert both to a string and concat
	if (kn_value_is_string(lhs)) {
		return kn_fn_mul_string(
			kn_value_as_string(lhs),
			(size_t) kn_value_to_number(args[1]));
	} else {
		kn_number_t number = kn_value_to_number(lhs);
		kn_value_free(lhs);

		return kn_value_new_number(
			number * kn_value_to_number(args[1]));
	}
});

DECLARE_FUNCTION(div, 2, '/', {
	kn_number_t dividend = kn_value_to_number(args[0]);
	kn_number_t divisor = kn_value_to_number(args[1]);

	if (divisor == 0)
		die("attempted to divide by zero");

	return kn_value_new_number(dividend / divisor);
});

DECLARE_FUNCTION(mod, 2, '%', {
	kn_number_t number = kn_value_to_number(args[0]);
	kn_number_t base = kn_value_to_number(args[1]);

	if (base == 0)
		die("attempted to modulo by zero");

	return kn_value_new_number(number % base);
});

DECLARE_FUNCTION(pow, 2, '^', {
	kn_number_t result = 1;
	kn_number_t base = kn_value_to_number(args[0]);
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

		for (; exponent > 0; --exponent) {
			result *= base;
		}
	}

	return kn_value_new_number(result);
});

DECLARE_FUNCTION(eql, 2, '?', {
	kn_value_t lhs = kn_value_run(args[0]);
	kn_value_t rhs = kn_value_run(args[1]);

	kn_value_t ret = kn_value_new_boolean(kn_value_eql(lhs, rhs));

	kn_value_free(lhs);
	kn_value_free(rhs);

	return ret;
});

DECLARE_FUNCTION(lth, 2, '<', {
	kn_value_t lhs = kn_value_run(args[0]);
	bool less;

	if (kn_value_is_string(lhs)) {
		const struct kn_string_t *lstr = kn_value_as_string(lhs);
		const struct kn_string_t *rstr = kn_value_to_string(args[1]);

		less = strcmp(lstr->str, rstr->str) < 0;

		kn_string_free(lstr);
		kn_string_free(rstr);
	} else if (kn_value_is_number(lhs)) {
		less = kn_value_as_number(lhs) < kn_value_to_number(args[1]);
	} else if (kn_value_is_boolean(lhs)) {
		less = !kn_value_to_boolean(args[1]) && lhs == KN_TRUE;
	} else {
		die("invalid value %d", lhs);
	}

	return kn_value_new_boolean(less);
});

DECLARE_FUNCTION(gth, 2, '>', {
	kn_value_t lhs = kn_value_run(args[0]);
	bool more;

	if (kn_value_is_string(lhs)) {
		const struct kn_string_t *lstr = kn_value_as_string(lhs);
		const struct kn_string_t *rstr = kn_value_to_string(args[1]);

		more = strcmp(lstr->str, rstr->str) > 0;

		kn_string_free(lstr);
		kn_string_free(rstr);
	} else if (kn_value_is_number(lhs)) {
		more = kn_value_as_number(lhs) > kn_value_to_number(args[1]);
	} else if (kn_value_is_boolean(lhs)) {
		more = kn_value_to_boolean(args[1]) && lhs == KN_FALSE;
	} else {
		die("invalid value %d", lhs);
	}

	return kn_value_new_boolean(more);
});

DECLARE_FUNCTION(and, 2, '&', {
	kn_value_t ret = kn_value_run(args[0]);

	// execute the RHS if the LHS is true.
	if (!kn_value_to_boolean(ret))
		return ret;

	kn_value_free(ret);
	return kn_value_run(args[1]);
});

DECLARE_FUNCTION(or, 2, '|', {
	kn_value_t ret = kn_value_run(args[0]);

	// execute the RHS if the LHS is true.
	if (kn_value_to_boolean(ret))
		return ret;

	kn_value_free(ret);
	return kn_value_run(args[1]);
});

DECLARE_FUNCTION(then, 2, ';', {
	kn_value_free(kn_value_run(args[0]));

	return kn_value_run(args[1]);
});


DECLARE_FUNCTION(assign, 2, '=', {
	kn_value_t ret;

	// if it's an identifier, special-case it where we don't evaluate it.
	if (kn_value_is_identifier(args[0])) {
		ret = kn_value_run(args[1]);
		kn_env_set(
			kn_value_as_identifier(args[0]), kn_value_clone(ret));
	} else {
		// otherwise, evaluate the expression, convert to a string,
		// and then use that as the identifier.
		const struct kn_string_t *ident = kn_value_to_string(args[0]);

		ret = kn_value_run(args[1]);
		kn_env_set(ident->str, kn_value_clone(ret));
		kn_string_free(ident);
	}

	return ret;
});


DECLARE_FUNCTION(while, 2, 'W', {
	while (kn_value_to_boolean(args[0]))
		(void) kn_value_run(args[1]);

	return KN_NULL;
});


DECLARE_FUNCTION(if, 3, 'I', {
	return kn_value_run(args[kn_value_to_boolean(args[0]) ? 1 : 2]);
});


DECLARE_FUNCTION(get, 3, 'G', {
	assert(args != NULL);

	const struct kn_string_t *string = kn_value_to_string(args[0]);
	size_t start = (size_t) kn_value_to_number(args[1]);
	size_t amnt = (size_t) kn_value_to_number(args[2]);
	kn_value_t ret;

	size_t length = strlen(string->str);

	if (length - 1 < start) {
		ret = kn_value_new_string(&KN_STRING_EMPTY);
	} else {
		char *substr = strndup(string->str + start, amnt);

		if (substr == NULL) {
			die("substring creation failed");
		}

		ret = kn_value_new_string(kn_string_new(substr));
	}

	kn_string_free(string);

	return ret;
});

DECLARE_FUNCTION(set, 4, 'S', {
	kn_value_t ret;

	const struct kn_string_t *string = kn_value_to_string(args[0]);
	size_t start = (size_t) kn_value_to_number(args[1]);
	kn_value_dump(args[2]);
	size_t amnt = (size_t) kn_value_to_number(args[2]);
	size_t length = strlen(string->str);
	const struct kn_string_t *substr = kn_value_to_string(args[3]);

	// if it's out of bounds, die.
	if (length < start) {
		die("index '%zu' out of bounds (length=%zu)", start, length);
	}

	// this could be made more efficient.
	char *retstr = xmalloc(length + strlen(substr->str) + 1);
	printf("retstr=%p, length = %zu, strlen(substr->str)=%zu\n",
		(void*)string, length, strlen(substr->str));
	memcpy(retstr, string->str, start);
	printf("start=%zu, string->str=[%s]\n", start, string->str);
	retstr[start] = '\0';
	printf("spot2\n");
	strcat(retstr + start, substr->str);
	printf("spot3a:\n\tretstr (%p)+\n\tstart (%zu)\n\t+strlen(substr->str) (%zu)\n\t=%p\n",
		(void *) retstr, start, strlen(substr->str), (void *)(retstr + start + strlen(substr->str)));
	printf("spot3b:\n\tstring->str (%p)+\n\tstart (%zu)\n\t+amnt (%zu)\n\t=%p\n",
		(void *) string->str, start, amnt, (void *)(string->str + start + amnt));
	strcat(retstr + start + strlen(substr->str), string->str + start + amnt);
	printf("spot4\n");

	ret = kn_value_new_string(kn_string_new(retstr));
	printf("spot5\n");

	kn_string_free(string);
	printf("spot6\n");
	kn_string_free(substr);
	printf("spot7\n");
	printf("/\n");

	return ret;
});
