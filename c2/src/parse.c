#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "value.h"
#include "parse.h"
#include "shared.h"
#include "function.h"
#include "env.h"

static int isparen(char c) {
	return c == ':'
		|| c == '(' || c == ')'
		|| c == '[' || c == ']'
		|| c == '{' || c == '}';
}

static int isident(char c) {
	return islower(c) || isdigit(c) || c == '_';
}

#define ADVANCE() do { ++*stream; } while(0)
#define PEEK() (**stream)
#define ADVANCE_PEEK() (*++*stream)
#define PEEK_ADVANCE() (*(*stream)++)


#ifdef COMPUTED_GOTOS
#define LABEL(x) x:
#define CASES10(a, ...)
#define CASES9(a, ...)
#define CASES8(a, ...)
#define CASES7(a, ...)
#define CASES6(a, ...)
#define CASES5(a, ...)
#define CASES4(a, ...)
#define CASES3(a, ...)
#define CASES2(a, ...)
#define CASES1(a)
#else
#define LABEL(x)
#define CASES10(a, ...)case a: CASES9(__VA_ARGS__)
#define CASES9(a, ...) case a: CASES8(__VA_ARGS__)
#define CASES8(a, ...) case a: CASES7(__VA_ARGS__)
#define CASES7(a, ...) case a: CASES6(__VA_ARGS__)
#define CASES6(a, ...) case a: CASES5(__VA_ARGS__)
#define CASES5(a, ...) case a: CASES4(__VA_ARGS__)
#define CASES4(a, ...) case a: CASES3(__VA_ARGS__)
#define CASES3(a, ...) case a: CASES2(__VA_ARGS__)
#define CASES2(a, ...) case a: CASES1(__VA_ARGS__)
#define CASES1(a) case a:
#endif

#define SYMBOL_FUNC(name, sym) \
	LABEL(function_##name) CASES1(sym) \
	function = &kn_fn_##name; \
	ADVANCE(); \
	goto parse_function

#define WORD_FUNC(name, sym) \
	LABEL(function_##name) CASES1(sym) \
	function = &kn_fn_##name; \
	goto parse_kw_function

kn_value_t kn_parse(register const char **stream) {

#ifdef COMPUTED_GOTOS
	static const void *LABELS[256] = {
		['\0'] = &&expected_token,
		[0x01 ... 0x08] = &&invalid,
		['\t' ... '\r'] = &&whitespace,
		[0x0e ... 0x1f] = &&invalid,
		[' ']  = &&whitespace,
		['!']  = &&function_not,
		['"']  = &&string,
		['#']  = &&comment,
		['$']  = &&invalid,
		['%']  = &&function_mod,
		['&']  = &&function_and,
		['\''] = &&string,
		['(']  = &&whitespace,
		[')']  = &&whitespace,
		['*']  = &&function_mul,
		['+']  = &&function_add,
		[',']  = &&invalid,
		['-']  = &&function_sub,
		['.']  = &&invalid,
		['/']  = &&function_div,
		['0' ... '9']  = &&number,
		[':']  = &&whitespace,
		[';']  = &&function_then,
		['<']  = &&function_lth,
		['=']  = &&function_assign,
		['>']  = &&function_gth,
		['?']  = &&function_eql,
		['@']  = &&invalid,
		['A']  = &&invalid,
		['B']  = &&function_block,
		['C']  = &&function_call,
		['D']  = &&function_dump,
		['E']  = &&function_eval,
		['F']  = &&literal_false,
		['G']  = &&function_get,
		['H']  = &&invalid,
		['I']  = &&function_if,
		['J']  = &&invalid,
		['K']  = &&invalid,
		['L']  = &&function_length,
		['M']  = &&invalid,
		['N']  = &&literal_null,
		['O']  = &&function_output,
		['P']  = &&function_prompt,
		['Q']  = &&function_quit,
		['R']  = &&function_random,
		['S']  = &&function_set,
		['T']  = &&literal_true,
		['U']  = &&invalid,
#ifdef KN_EXT_VALUE
		['V']  = &&function_value,
#else
		['V']  = &&invalid,
#endif /* KN_EXT_VALUE */
		['W']  = &&function_while,
		['X']  = &&invalid,
		['Y']  = &&invalid,
		['Z']  = &&invalid,
		['[']  = &&whitespace,
		['\\'] = &&invalid,
		[']']  = &&whitespace,
		['^']  = &&function_pow,
		['_']  = &&identifier,
		['`']  = &&function_system,
		['a' ... 'z'] = &&identifier,
		['{']  = &&whitespace,
		['|']  = &&function_or,
		['}']  = &&whitespace,
		['~']  = &&invalid,
		[0x7f ... 0xff] = &&invalid
	};
#endif /* COMPUTED_GOTOS */

	char c;
	struct kn_function_t *function;

	assert(stream != NULL);
	assert(*stream != NULL);

start:
	c = PEEK();
#ifdef COMPUTED_GOTOS
	goto *LABELS[(size_t) c];
#else
	switch (c) {
#endif /* COMPUTED_GOTOS */


LABEL(comment)
CASES1('#')
	while ((c = ADVANCE_PEEK()) != '\n')
		if (c == '\0')
			goto expected_token;

	// fallthrough, because we're currently a whitespace character (`\n`)

LABEL(whitespace)
CASES6('\t', '\n', '\v', '\f', '\r', ' ')
CASES7('(', ')', '[', ']', '{', '}', ':')
	while (isspace(c = ADVANCE_PEEK()) || isparen(c));
	goto start;

LABEL(number)
CASES10('0', '1', '2', '3', '4', '5', '6', '7', '8', '9')
{
	kn_number_t number = (c - '0');

	while (isdigit(c = ADVANCE_PEEK()))
		number = number * 10 + (c - '0');

	return kn_value_new_number(number);
}

LABEL(identifier)
CASES10('a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j')
CASES10('k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't')
CASES7( 'u', 'v', 'w', 'x', 'y', 'z', '_')
{	// simply find the start and end of the identifier
	const char *start = *stream;

	while (isident(ADVANCE_PEEK()));

	return kn_value_new_variable(
		kn_env_fetch(strndup(start, *stream - start), true));
}

LABEL(string)
CASES2('\'', '\"')
{
	char quote = c;
	ADVANCE();
	const char *start = *stream;

	while ((c = PEEK_ADVANCE()) != quote)
		if (c == '\0')
			die("unterminated quote encountered: '%s'", start);

	size_t length = *stream - start - 1;

	if (!length)
		return kn_value_new_string(&KN_STRING_EMPTY);

	char *str = strndup(start, length);

	return kn_value_new_string(kn_string_new(str, length));
}

LABEL(literal_true)
CASES1('T')
	while(isupper(ADVANCE_PEEK()));
	return KN_TRUE;

LABEL(literal_false)
CASES1('F')
	while(isupper(ADVANCE_PEEK()));
	return KN_FALSE;

LABEL(literal_null)
CASES1('N')
	while(isupper(ADVANCE_PEEK()));
	return KN_NULL;

SYMBOL_FUNC(not, '!');
SYMBOL_FUNC(add, '+');
SYMBOL_FUNC(sub, '-');
SYMBOL_FUNC(mul, '*');
SYMBOL_FUNC(div, '/');
SYMBOL_FUNC(mod, '%');
SYMBOL_FUNC(pow, '^');
SYMBOL_FUNC(eql, '?');
SYMBOL_FUNC(lth, '<');
SYMBOL_FUNC(gth, '>');
SYMBOL_FUNC(and, '&');
SYMBOL_FUNC(or, '|');
SYMBOL_FUNC(then, ';');
SYMBOL_FUNC(assign, '=');
SYMBOL_FUNC(system, '`');
WORD_FUNC(block, 'B');
WORD_FUNC(call, 'C');
WORD_FUNC(dump, 'D');
WORD_FUNC(eval, 'E');
WORD_FUNC(get, 'G');
WORD_FUNC(if, 'I');
WORD_FUNC(length, 'L');
WORD_FUNC(output, 'O');
WORD_FUNC(prompt, 'P');
WORD_FUNC(quit, 'Q');
WORD_FUNC(random, 'R');
WORD_FUNC(set, 'S');
WORD_FUNC(while, 'W');

#ifdef KN_EXT_VALUE
WORD_FUNC(value, 'V');
#endif

parse_kw_function:
	while (isupper(ADVANCE_PEEK()));
	// fallthrough

parse_function:
{
	size_t arity = function->arity;
	struct kn_ast_t *ast = xmalloc(sizeof(struct kn_ast_t)
		+ sizeof(kn_value_t [arity]));

	ast->func = function;
	ast->refcount = 1;

#ifdef DYAMIC_THEN_ARGC
	if (function != &kn_fn_then) {
	ast->argc = arity;
#endif

	for (size_t i = 0; i < arity; ++i) {
		if ((ast->args[i] = kn_parse(stream)) == KN_UNDEFINED) {
			die("unable to parse argument %d for function '%c'", i,
				function->name);
		}
	}

#ifdef DYAMIC_THEN_ARGC
	goto parse_function_end;
	}

	ast->argc = 0;

	if (arity == 0)
		goto parse_function_end;

	if ((ast->args[ast->argc++] = kn_parse(stream)) == KN_UNDEFINED)
		die("unable to parse argument 0 for function '%c'", function->name);

	unsigned cap = arity;
	do {
		c = PEEK();
		if (c == '#')
			while ((c = ADVANCE_PEEK()) != '\n')
				if (c == '\0')
					break;
		if (isspace(c))
			while (isspace(c = ADVANCE_PEEK()) || isparen(c));

		if (PEEK() != function->name) {
			if ((ast->args[ast->argc++] = kn_parse(stream)) == KN_UNDEFINED)
				die("unable to parse argument %d for function '%c'",
					ast->argc-1, function->name);
			break;
		}

		ADVANCE();
		ast->args[ast->argc++] = kn_parse(stream);

		if (ast->argc == cap)  {
			ast = xrealloc(ast, sizeof(struct kn_ast_t) + sizeof(
				kn_value_t [cap *= 2]));
		}
	} while (1);

	ast = xrealloc(ast, sizeof(struct kn_ast_t) + sizeof(
		kn_value_t [ast->argc + 1]));

	ast->args[ast->argc] = KN_UNDEFINED;

parse_function_end:
#endif

	return kn_value_new_ast(ast);
}

expected_token:
CASES1('\0')
	return KN_UNDEFINED;

LABEL(invalid)
#ifndef COMPUTED_GOTOS
	default:
#endif

	die("unknown token start '%c'", c);

#ifndef COMPUTED_GOTOS
	}
#endif
}
