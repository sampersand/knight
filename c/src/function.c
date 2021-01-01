#include <string.h>
#include <time.h>
#include "ast.h"
#include "knight.h"
#include "shared.h"
#include "function.h"

struct kn_value_t kn_fn_assign(const struct kn_ast_t *args) {
	struct kn_value_t ret;
	if (args[0].kind == KN_TT_IDENT) {
		ret = kn_ast_run(&args[1]);
		kn_env_set(args[0].ident, kn_value_clone(&ret));
	} else {
		struct kn_value_t var = kn_ast_run(&args[0]);
		struct kn_string_t ident = kn_value_to_string(&var);

		ret = kn_ast_run(&args[1]);
		kn_env_set(ident.str, kn_value_clone(&ret));

		kn_string_free(&ident);
		kn_value_free(&var);
	}

	return ret;
}

struct kn_value_t kn_fn_block(const struct kn_ast_t *args) {
	struct kn_ast_t *ptr = xmalloc(sizeof(struct kn_ast_t));

	*ptr = kn_ast_clone(&args[0]);

	return kn_value_new_ast(ptr);
}

struct kn_value_t kn_fn_while(const struct kn_ast_t *args) {
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
	struct kn_value_t condition = kn_ast_run(&args[0]);

	int which_arg = kn_value_to_boolean(&condition) ? 1 : 2;
	struct kn_value_t ret = kn_ast_run(&args[which_arg]);

	kn_value_free(&condition);
	return ret;
}

struct kn_value_t kn_fn_and(const struct kn_ast_t *args) {
	struct kn_value_t ret = kn_ast_run(&args[0]);

	if (kn_value_to_boolean(&ret)) {
		kn_value_free(&ret);
		ret = kn_ast_run(&args[1]);
	}

	return ret;
}

struct kn_value_t kn_fn_or(const struct kn_ast_t *args) {
	struct kn_value_t ret = kn_ast_run(&args[0]);

	if (!kn_value_to_boolean(&ret)) {
		kn_value_free(&ret);
		ret = kn_ast_run(&args[1]);
	}

	return ret;
}

struct kn_value_t kn_fn_prompt(const struct kn_ast_t *args) {
	(void) args;

	size_t linelen;
	char *lineptr = fgetln(stdin, &linelen);

	if (lineptr == NULL) {
		if (feof(stdin)) {
			return kn_value_new_string(kn_string_intern(""));
		} else {
			perror("unable to read line");
		}
	}

	lineptr = xrealloc(lineptr, linelen + 1);
	lineptr[linelen + 1] = '\0';

	return kn_value_new_string(kn_string_new(lineptr));
}

struct kn_value_t kn_fn_rand(const struct kn_ast_t *args) {
	(void) args;

	static int SEEDED = 0;

	if (!SEEDED) {
		SEEDED = 1;
		srand(time(NULL));
	}

	return kn_value_new_integer((kn_integer_t) rand());
}

struct kn_value_t kn_fn_true(const struct kn_ast_t *args) {
	(void) args;

	return kn_value_new_boolean(true);
}

struct kn_value_t kn_fn_false(const struct kn_ast_t *args) {
	(void) args;

	return kn_value_new_boolean(false);
}

struct kn_value_t kn_fn_null(const struct kn_ast_t *args) {
	(void) args;

	return kn_value_new_null();
}

struct kn_value_t kn_fn_eval(const struct kn_ast_t *args) {
	struct kn_value_t arg = kn_ast_run(&args[0]);
	struct kn_string_t string = kn_value_to_string(&arg);

	struct kn_value_t ret = kn_run(string.str);

	kn_string_free(&string);
	kn_value_free(&arg);

	return ret;
}

struct kn_value_t kn_fn_call(const struct kn_ast_t *args) {
	struct kn_value_t arg = kn_ast_run(&args[0]);

	if (arg.kind != KN_VT_AST) {
		die("Unable to call '%d's; only block supported", arg.kind);
	}

	struct kn_value_t ret = kn_ast_run(arg.ast);

	kn_value_free(&arg);

	return ret;
}

struct kn_value_t kn_fn_system(const struct kn_ast_t *args) {
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

	while ((tmp = fread(result + len, 1, cap - len, stream))) {
		len += tmp;
		if (len == cap) {
			cap *= 2;
			result = xrealloc(result, cap);
		}
	}

	if (ferror(stream)) {
		die("unable to read command stream");
	}

	result = xrealloc(result, len + 1);
	result[len] = '\0';

	if (pclose(stream) == -1) {
		die("unable to close command stream.");
	}

	ret = kn_value_new_string(kn_string_new(result));

	kn_string_free(&command);
	kn_value_free(&arg0);

	return ret;
}

struct kn_value_t kn_fn_quit(const struct kn_ast_t *args) {
	struct kn_value_t arg = kn_ast_run(&args[0]);

	exit((int) kn_value_to_integer(&arg));
}

struct kn_value_t kn_fn_not(const struct kn_ast_t *args) {
	struct kn_value_t arg = kn_ast_run(&args[0]);

	struct kn_value_t ret = kn_value_new_boolean(
		!kn_value_to_boolean(&arg));

	kn_value_free(&arg);

	return ret;
}

struct kn_value_t kn_fn_length(const struct kn_ast_t *args) {
	struct kn_value_t arg = kn_ast_run(&args[0]);
	struct kn_string_t string = kn_value_to_string(&arg);

	struct kn_value_t ret = kn_value_new_integer(strlen(string.str));

	kn_string_free(&string);
	kn_value_free(&arg);

	return ret;
}

struct kn_value_t kn_fn_output(const struct kn_ast_t *args) {
	struct kn_value_t arg = kn_ast_run(&args[0]);
	struct kn_string_t string = kn_value_to_string(&arg);

	// right here we're casting away the const.
	// this is because we might need to replace the penult character
	// with a `\0` if it's a backslash to prevent the printing of a
	// newline. however, we replace it on the next line, so it's ok.
	char *str = (char *) string.str;
	size_t len = strlen(str);
	char *penult = str + len - 1;

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
	DEBUG_ASSERT(lhs.kind == KN_VT_STRING, "non-string type '%d' given?",
		lhs.kind);

	bool is_rhs_not_a_string = rhs.kind != KN_VT_STRING;
	struct kn_string_t rstring =
		is_rhs_not_a_string ? kn_value_to_string(&rhs) : rhs.string;

	if (strlen(lhs.string.str) == 0) {
		kn_value_free(&lhs);

		if (is_rhs_not_a_string) {
			kn_value_free(&rhs);
			rhs = kn_value_new_string(rstring);
		}

		return rhs;
	} else if (strlen(rstring.str) == 0) {
		kn_value_free(&rhs);

		if (is_rhs_not_a_string) {
			kn_string_free(&rstring);
		}

		return lhs;

	}

	char *ret = xmalloc(strlen(lhs.string.str) + strlen(rstring.str) + 1);

	strcpy(ret, lhs.string.str);
	strcat(ret, rstring.str);

	kn_value_free(&lhs);
	kn_value_free(&rhs);

	if (is_rhs_not_a_string) {
		kn_string_free(&rstring);
	}

	return kn_value_new_string(kn_string_new(ret));
}

struct kn_value_t kn_fn_add(const struct kn_ast_t *args) {
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
	DEBUG_ASSERT(lhs.kind == KN_VT_STRING, "non-string type '%d' given?",
		lhs.kind);

	// if we have an empty string, return early.
	if (strlen(lhs.string.str) == 0) {
		kn_value_free(&rhs);
		return lhs;
	}

	kn_integer_t amnt = kn_value_to_integer(&rhs);
	kn_value_free(&rhs); // we don't need it anymore, we just need its amnt.

	if (amnt == 0) {
		kn_value_free(&lhs);
		return kn_value_new_string(kn_string_intern(""));
	} else if (amnt == 1) {
		return lhs;
	}

	if (amnt != (kn_integer_t) (size_t) amnt) {
		die("multiplicand '%jd' is too large!", amnt);
	}

	char *string = xmalloc(1 + strlen(lhs.string.str) * (size_t) amnt);

	for (; amnt != 0; --amnt) {
		// TODO: this can be made more efficient by calculating the
		// new offset ourself each time.
		strcat(string, lhs.string.str);
	}

	kn_value_free(&lhs);
	return kn_value_new_string(kn_string_new(string));
}

struct kn_value_t kn_fn_mul(const struct kn_ast_t *args) {
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
	struct kn_value_t lhs = kn_ast_run(&args[0]);
	struct kn_value_t rhs = kn_ast_run(&args[1]);

	kn_integer_t result = 1;
	kn_integer_t base = kn_value_to_integer(&lhs);
	kn_integer_t exponent = kn_value_to_integer(&rhs);

	if (base != 0) {
		for (; exponent != 0; --exponent) {
			result *= base;
		}
	}

	kn_value_free(&lhs);
	kn_value_free(&rhs);

	return kn_value_new_integer(result);
}

struct kn_value_t kn_fn_eql(const struct kn_ast_t *args) {
	struct kn_value_t lhs = kn_ast_run(&args[0]);
	struct kn_value_t rhs = kn_ast_run(&args[1]);
	bool is_eql;

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
		// ASTs are only equal if theyre the _exact same_ object.
		is_eql = lhs.ast == rhs.ast;
		break;

	default:
		bug("unknown kind '%d'", lhs.kind);
	}

free_and_return:
	kn_value_free(&lhs);
	kn_value_free(&rhs);

	return kn_value_new_boolean(is_eql);
}

static int kn_value_cmp(
	const struct kn_value_t lhs,
	const struct kn_value_t rhs
) {
	switch(lhs.kind) {
	case KN_VT_NULL:
		die("cannot compare NULL");

	case KN_VT_BOOLEAN:
		return lhs.boolean - kn_value_to_boolean(&rhs);

	case KN_VT_INTEGER:
		return lhs.integer - kn_value_to_integer(&rhs);

	case KN_VT_STRING: {
		struct kn_string_t rstring = kn_value_to_string(&rhs);
		int cmp = strcmp(lhs.string.str, rhs.string.str);
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
	struct kn_value_t lhs = kn_ast_run(&args[0]);
	struct kn_value_t rhs = kn_ast_run(&args[1]);

	struct kn_value_t ret = kn_value_new_boolean(
		kn_value_cmp(lhs, rhs) < 0);

	kn_value_free(&lhs);
	kn_value_free(&rhs);

	return ret;
}

struct kn_value_t kn_fn_gth(const struct kn_ast_t *args) {
	struct kn_value_t lhs = kn_ast_run(&args[0]);
	struct kn_value_t rhs = kn_ast_run(&args[1]);

	struct kn_value_t ret = kn_value_new_boolean(
		kn_value_cmp(lhs, rhs) > 0);

	kn_value_free(&lhs);
	kn_value_free(&rhs);

	return ret;
}

struct kn_value_t kn_fn_then(const struct kn_ast_t *args) {
	struct kn_value_t lhs = kn_ast_run(&args[0]);
	struct kn_value_t rhs = kn_ast_run(&args[1]);

	kn_value_free(&lhs);

	return rhs;
}

struct kn_value_t kn_fn_get(const struct kn_ast_t *args) {
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

		VERIFY_NOT_NULL(substr, "substring creation failed");

		ret = kn_value_new_string(kn_string_new(substr));
	}

	kn_string_free(&string);
	kn_value_free(&arg2);
	kn_value_free(&arg1);
	kn_value_free(&arg0);

	return ret;
}

struct kn_value_t kn_fn_set(const struct kn_ast_t *args) {
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
	char *retstr = xmalloc(length + strlen(substr.str));
	memcpy(retstr, string.str, start);
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

static struct kn_function_t FUNCTIONS[0xff] = {
	['P'] = { 0, kn_fn_prompt },
	['R'] = { 0, kn_fn_rand },
	['T'] = { 0, kn_fn_true },
	['F'] = { 0, kn_fn_false },
	['N'] = { 0, kn_fn_null },

	['B'] = { 1, kn_fn_block },
	['E'] = { 1, kn_fn_eval },
	['C'] = { 1, kn_fn_call },
	['`'] = { 1, kn_fn_system },
	['Q'] = { 1, kn_fn_quit },
	['!'] = { 1, kn_fn_not },
	['L'] = { 1, kn_fn_length },
	['O'] = { 1, kn_fn_output },

	['+'] = { 2, kn_fn_add },
	['-'] = { 2, kn_fn_sub },
	['*'] = { 2, kn_fn_mul },
	['/'] = { 2, kn_fn_div },
	['%'] = { 2, kn_fn_mod },
	['^'] = { 2, kn_fn_pow },
	['?'] = { 2, kn_fn_eql },
	['<'] = { 2, kn_fn_lth },
	['>'] = { 2, kn_fn_gth },
	[';'] = { 2, kn_fn_then },
	['='] = { 2, kn_fn_assign },
	['W'] = { 2, kn_fn_while },
	['&'] = { 2, kn_fn_and },
	['|'] = { 2, kn_fn_or },

	['I'] = { 3, kn_fn_if },
	['G'] = { 3, kn_fn_get },

	['S'] = { 4, kn_fn_set },
};

const struct kn_function_t *kn_fn_fetch(char name) {
	return FUNCTIONS[(int) name].func == NULL ? NULL : &FUNCTIONS[(int) name];
}

void kn_fn_register_func(char name, struct kn_function_t func) {
	FUNCTIONS[(int) name] = func;
}
