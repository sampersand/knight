#include <assert.h> /* assert */
#include <stddef.h> /* size_t */
#include <ctype.h>  /* isspace, isdigit, islower, isupper */
#include <string.h> /* strndup, memcpy */

#include "parse.h"    /* prototypes, kn_value, kn_number, kn_value_new_number,
                         kn_value_new_variable, kn_value_new_string,
                         kn_value_new_ast, kn_string_alloc, KN_UNDEFINED,
                         KN_TRUE, KN_FALSE, KN_NULL */
#include "string.h"   /* kn_string, kn_string_empty, kn_string_deref */
#include "ast.h"      /* kn_ast, kn_ast_alloc */
#include "shared.h"   /* die */
#include "function.h" /* kn_function, <all the function definitions> */
#include "env.h"      /* kn_variable, kn_env_fetch */

// Check to see if the character is considered whitespace to Knight.
static int iswhitespace(char c) {
	return isspace(c) || c == ':'
		|| c == '(' || c == ')'
		|| c == '[' || c == ']'
		|| c == '{' || c == '}';
}

// Checks to see if the character is part of a word function body.
static int iswordfunc(char c) {
	return isupper(c) || c == '_';
}

// Check to see if the character is an identifier character.
static int isident(char c) {
	return islower(c) || isdigit(c) || c == '_';
}

// Macros for advancing or looking at the stream.
#define ADVANCE() do { ++*stream; } while(0)
#define PEEK() (**stream)
#define ADVANCE_PEEK() (*++*stream)
#define PEEK_ADVANCE() (*(*stream)++)

// Macros used either for computed gotos or switch statements (the switch
// statement is only used when `KN_COMPUTED_GOTOS` is not defined.)
#ifdef KN_COMPUTED_GOTOS
# define LABEL(x) x:
# define CASES10(a, ...)
# define CASES9(a, ...)
# define CASES8(a, ...)
# define CASES7(a, ...)
# define CASES6(a, ...)
# define CASES5(a, ...)
# define CASES4(a, ...)
# define CASES3(a, ...)
# define CASES2(a, ...)
# define CASES1(a)
#else
# define LABEL(x)
# define CASES10(a, ...)case a: CASES9(__VA_ARGS__)
# define CASES9(a, ...) case a: CASES8(__VA_ARGS__)
# define CASES8(a, ...) case a: CASES7(__VA_ARGS__)
# define CASES7(a, ...) case a: CASES6(__VA_ARGS__)
# define CASES6(a, ...) case a: CASES5(__VA_ARGS__)
# define CASES5(a, ...) case a: CASES4(__VA_ARGS__)
# define CASES4(a, ...) case a: CASES3(__VA_ARGS__)
# define CASES3(a, ...) case a: CASES2(__VA_ARGS__)
# define CASES2(a, ...) case a: CASES1(__VA_ARGS__)
# define CASES1(a) case a:
#endif /* KN_COMPUTED_GOTOS */

// Used for functions which are only a single character, eg `+`.
#define SYMBOL_FUNC(name, sym) \
	LABEL(function_##name) CASES1(sym) \
	function = &kn_fn_##name; \
	ADVANCE(); \
	goto parse_function

// Used for functions which are word functions (and can be multiple characters).
#define WORD_FUNC(name, sym) \
	LABEL(function_##name) CASES1(sym) \
	function = &kn_fn_##name; \
	goto parse_kw_function

kn_value kn_parse(register const char **stream) {

// the global lookup table, which is used for the slightly-more-efficient, ut
// mnon-standard computed gotos version of the parser.
#ifdef KN_COMPUTED_GOTOS
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
		['S']  = &&function_substitute,
		['T']  = &&literal_true,
		['U']  = &&invalid,

# ifdef KN_EXT_VALUE
		['V']  = &&function_value,
# else
		['V']  = &&invalid,
# endif /* KN_EXT_VALUE */

		['W']  = &&function_while,
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

#ifdef KN_EXT_NEGATE
		['~']  = &&function_negate,
#else
		['~']  = &&invalid,
#endif /* KN_EXT_NEGATE */

		[0x7f ... 0xff] = &&invalid
	};
#endif /* KN_COMPUTED_GOTOS */

	char c;
	const struct kn_function *function;

	assert(stream != NULL);
	assert(*stream != NULL);

start:
	c = PEEK();

#ifdef KN_COMPUTED_GOTOS
	goto *LABELS[(size_t) c];
#else
	switch (c) {
#endif /* KN_COMPUTED_GOTOS */

LABEL(comment)
CASES1('#')
	while ((c = ADVANCE_PEEK()) != '\n') {

#ifndef KN_RECKLESS
		// we hit end of stream, but we were looking for a token (as
		// otherwise we wouldn't be parsing values).
		if (c == '\0')
			goto expected_token;
#endif /* !KN_RECKLESS */

	}

	assert(c == '\n');
	// fallthrough, because we're currently a whitespace character (`\n`)

LABEL(whitespace)
CASES6('\t', '\n', '\v', '\f', '\r', ' ')
CASES7('(', ')', '[', ']', '{', '}', ':')
	while (iswhitespace(ADVANCE_PEEK()));
	goto start; // go find the next token to return.

LABEL(number)
CASES10('0', '1', '2', '3', '4', '5', '6', '7', '8', '9')
{
	kn_number number = (c - '0');

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

	char *identifier = strndup(start, *stream - start);
	struct kn_variable *variable = kn_env_fetch(identifier, true);
	return kn_value_new_variable(variable);
}

LABEL(string)
CASES2('\'', '\"')
{
	char quote = c;
	ADVANCE();
	const char *start = *stream;

	while (quote != (c = PEEK_ADVANCE())) {

#ifndef KN_RECKLESS
		if (c == '\0')
			die("unterminated quote encountered: '%s'", start);
#endif /* !KN_RECKLESS */

	}

	size_t length = *stream - start - 1;

	// optimize for the empty string
	if (!length)
		return kn_value_new_string(&kn_string_empty);

	struct kn_string *string = kn_string_alloc(length);
	memcpy(kn_string_deref(string), start, length);

	kn_string_deref(string)[length] = '\0';
	return kn_value_new_string(string);
}

LABEL(literal_true)
CASES1('T')
	while(iswordfunc(ADVANCE_PEEK()));
	return KN_TRUE;

LABEL(literal_false)
CASES1('F')
	while(iswordfunc(ADVANCE_PEEK()));
	return KN_FALSE;

LABEL(literal_null)
CASES1('N')
	while(iswordfunc(ADVANCE_PEEK()));
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

#ifdef KN_EXT_NEGATE
SYMBOL_FUNC(negate, '~');
#endif /* KN_EXT_NEGATE */

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
WORD_FUNC(substitute, 'S');
WORD_FUNC(while, 'W');

#ifdef KN_EXT_VALUE
WORD_FUNC(value, 'V');
#endif /* KN_EXT_VALUE */

parse_kw_function:
	while (iswordfunc(ADVANCE_PEEK()));

	// fallthrough to parsing a normal function

parse_function:
{
	size_t arity = function->arity;
	struct kn_ast *ast = kn_ast_alloc(arity);

	ast->func = function;
	ast->refcount = 1;


	for (size_t i = 0; i < arity; ++i) {
		ast->args[i] = kn_parse(stream);

#ifndef KN_RECKLESS
		if (ast->args[i] == KN_UNDEFINED)
			die("unable to parse argument %d for function '%c'",
				i, function->name);
#endif /* !KN_RECKLESS */
	}

	return kn_value_new_ast(ast);
}

expected_token:
CASES1('\0')
	return KN_UNDEFINED;

LABEL(invalid)
#ifndef KN_COMPUTED_GOTOS
	default:
#endif /* !KN_COMPUTED_GOTOS */

	die("unknown token start '%c'", c);

#ifndef KN_COMPUTED_GOTOS
	}
#endif /* !KN_COMPUTED_GOTOS */
}
