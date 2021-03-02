#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "value.h"
#include "parse.h"
#include "shared.h"
#include "function.h"
#include "ast.h"

#define ADVANCE() do { ++*stream; } while(0)
#define PEEK() (**stream)
#define ADVANCE_PEEK() (*++*stream)
#define PEEK_ADVANCE() (*(*stream)++)

static int isparen(char c) {
	return c == ':'
		|| c == '(' || c == ')'
		|| c == '[' || c == ']'
		|| c == '{' || c == '}';
}

static int isident(char c) {
	return islower(c) || isdigit(c) || c == '_';
}

kn_value_t kn_parse(register const char **stream) {
	assert(stream != NULL);
	assert(*stream != NULL);
	static const void * labels[256] = {
		['\0'] = &&expected_token,
		[0x01 ... 0x08] = &&invalid,
		['\t'] = &&whitespace,
		['\n'] = &&whitespace,
		['\v'] = &&whitespace,
		['\f'] = &&whitespace,
		['\r'] = &&whitespace,
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
		['V']  = &&invalid,
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
		[0x7f ... 0xff] = &&invalid,
	};

	char c;
	struct kn_function_t *function;

start:
	goto *labels[(size_t) (c = PEEK())];

comment:
	while ((c = ADVANCE_PEEK()) != '\n')
		if (c == '\0')
			goto expected_token;
	// fallthrough, because we're currently a whitespace character (`\n`)

whitespace:
	// __attribute__((hot));

	while (isspace(c = ADVANCE_PEEK()) || isparen(c));
	goto start;

number: {
	kn_number_t number = (c - '0');

	while (isdigit(c = ADVANCE_PEEK()))
		number = number * 10 + (c - '0');

	return kn_value_new_number(number);
}

identifier: {
	// simply find the start and end of the identifier
	const char *start = *stream;

	while (isident(ADVANCE_PEEK()));

	return kn_value_new_identifier(kn_string_emplace(start, *stream - start));
}

string: {
	char quote = c;
	ADVANCE();
	const char *start = *stream;

	while ((c = PEEK_ADVANCE()) != quote)
		if (c == '\0')
			die("unterminated quote encountered: '%s'", start);

	size_t length = *stream - start - 1;

	return kn_value_new_string(
		length
		? kn_string_emplace(start, length)
		: &KN_STRING_EMPTY);

}

literal_true:
	while(isupper(ADVANCE_PEEK()));
	return KN_TRUE;

literal_false:
	while(isupper(ADVANCE_PEEK()));
	return KN_FALSE;

literal_null:
	while(isupper(ADVANCE_PEEK()));
	return KN_NULL;

function_not:    function = &kn_fn_not;    ADVANCE(); goto parse_function;
function_add:    function = &kn_fn_add;    ADVANCE(); goto parse_function;
function_sub:    function = &kn_fn_sub;    ADVANCE(); goto parse_function;
function_mul:    function = &kn_fn_mul;    ADVANCE(); goto parse_function;
function_div:    function = &kn_fn_div;    ADVANCE(); goto parse_function;
function_mod:    function = &kn_fn_mod;    ADVANCE(); goto parse_function;
function_pow:    function = &kn_fn_pow;    ADVANCE(); goto parse_function;
function_eql:    function = &kn_fn_eql;    ADVANCE(); goto parse_function;
function_lth:    function = &kn_fn_lth;    ADVANCE(); goto parse_function;
function_gth:    function = &kn_fn_gth;    ADVANCE(); goto parse_function;
function_and:    function = &kn_fn_and;    ADVANCE(); goto parse_function;
function_or:     function = &kn_fn_or;     ADVANCE(); goto parse_function;
function_then:   function = &kn_fn_then;   ADVANCE(); goto parse_function;
function_assign: function = &kn_fn_assign; ADVANCE(); goto parse_function;
function_system: function = &kn_fn_system; ADVANCE(); goto parse_function;
function_block:  function = &kn_fn_block;  goto parse_kw_function;
function_call:   function = &kn_fn_call;   goto parse_kw_function;
function_dump:   function = &kn_fn_dump;   goto parse_kw_function;
function_eval:   function = &kn_fn_eval;   goto parse_kw_function;
function_get:    function = &kn_fn_get;    goto parse_kw_function;
function_if:     function = &kn_fn_if;     goto parse_kw_function;
function_length: function = &kn_fn_length; goto parse_kw_function;
function_output: function = &kn_fn_output; goto parse_kw_function;
function_prompt: function = &kn_fn_prompt; goto parse_kw_function;
function_quit:   function = &kn_fn_quit;   goto parse_kw_function;
function_random: function = &kn_fn_random; goto parse_kw_function;
function_set:    function = &kn_fn_set;    goto parse_kw_function;
function_while:  function = &kn_fn_while;  goto parse_kw_function;

parse_kw_function:
	while (isupper(ADVANCE_PEEK()));
	// fallthrough

parse_function: {
	size_t arity = function->arity;
	struct kn_ast_t *ast = xmalloc(
		sizeof(struct kn_ast_t) + sizeof(kn_value_t [arity]));

	ast->func = function;
	ast->refcount = 1;

	for (size_t i = 0; i < arity; ++i)
		ast->args[i] = kn_parse(stream);

	return kn_value_new_ast(ast);
}


expected_token:
	// __attribute__((cold));
	die("unexpected end of stream");

invalid:
	// __attribute__((cold));
	die("unknown token start '%c'", c);
}
