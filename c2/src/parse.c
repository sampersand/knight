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

kn_value_t kn_parse(const char **stream) {
	static const void * labels[256] = {
		['\0'] = &&expected_token,
		[0x01] = &&invalid,
		[0x02] = &&invalid,
		[0x03] = &&invalid,
		[0x04] = &&invalid,
		[0x05] = &&invalid,
		[0x06] = &&invalid,
		[0x07] = &&invalid,
		[0x08] = &&invalid,
		['\t'] = &&whitespace,
		['\n'] = &&whitespace,
		['\v'] = &&whitespace,
		['\f'] = &&whitespace,
		['\r'] = &&whitespace,
		[0x0E] = &&invalid,
		[0x0F] = &&invalid,
		[0x10] = &&invalid,
		[0x11] = &&invalid,
		[0x12] = &&invalid,
		[0x13] = &&invalid,
		[0x14] = &&invalid,
		[0x15] = &&invalid,
		[0x16] = &&invalid,
		[0x17] = &&invalid,
		[0x18] = &&invalid,
		[0x19] = &&invalid,
		[0x1A] = &&invalid,
		[0x1B] = &&invalid,
		[0x1C] = &&invalid,
		[0x1D] = &&invalid,
		[0x1E] = &&invalid,
		[0x1F] = &&invalid,
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
		['0']  = &&number,
		['1']  = &&number,
		['2']  = &&number,
		['3']  = &&number,
		['4']  = &&number,
		['5']  = &&number,
		['6']  = &&number,
		['7']  = &&number,
		['8']  = &&number,
		['9']  = &&number,
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
		['a']  = &&identifier,
		['b']  = &&identifier,
		['c']  = &&identifier,
		['d']  = &&identifier,
		['e']  = &&identifier,
		['f']  = &&identifier,
		['g']  = &&identifier,
		['h']  = &&identifier,
		['i']  = &&identifier,
		['j']  = &&identifier,
		['k']  = &&identifier,
		['l']  = &&identifier,
		['m']  = &&identifier,
		['n']  = &&identifier,
		['o']  = &&identifier,
		['p']  = &&identifier,
		['q']  = &&identifier,
		['r']  = &&identifier,
		['s']  = &&identifier,
		['t']  = &&identifier,
		['u']  = &&identifier,
		['v']  = &&identifier,
		['w']  = &&identifier,
		['x']  = &&identifier,
		['y']  = &&identifier,
		['z']  = &&identifier,
		['{']  = &&whitespace,
		['|']  = &&function_or,
		['}']  = &&whitespace,
		['~']  = &&invalid,
		[0x7f] = &&invalid,
		[0x80] = &&invalid, [0x81] = &&invalid, [0x82] = &&invalid,
		[0x83] = &&invalid, [0x84] = &&invalid, [0x85] = &&invalid,
		[0x86] = &&invalid, [0x87] = &&invalid, [0x88] = &&invalid,
		[0x89] = &&invalid, [0x8a] = &&invalid, [0x8b] = &&invalid,
		[0x8c] = &&invalid, [0x8d] = &&invalid, [0x8e] = &&invalid,
		[0x8f] = &&invalid, [0x90] = &&invalid, [0x91] = &&invalid,
		[0x92] = &&invalid, [0x93] = &&invalid, [0x94] = &&invalid,
		[0x95] = &&invalid, [0x96] = &&invalid, [0x97] = &&invalid,
		[0x98] = &&invalid, [0x99] = &&invalid, [0x9a] = &&invalid,
		[0x9b] = &&invalid, [0x9c] = &&invalid, [0x9d] = &&invalid,
		[0x9e] = &&invalid, [0x9f] = &&invalid, [0xa0] = &&invalid,
		[0xa1] = &&invalid, [0xa2] = &&invalid, [0xa3] = &&invalid,
		[0xa4] = &&invalid, [0xa5] = &&invalid, [0xa6] = &&invalid,
		[0xa7] = &&invalid, [0xa8] = &&invalid, [0xa9] = &&invalid,
		[0xaa] = &&invalid, [0xab] = &&invalid, [0xac] = &&invalid,
		[0xad] = &&invalid, [0xae] = &&invalid, [0xaf] = &&invalid,
		[0xb0] = &&invalid, [0xb1] = &&invalid, [0xb2] = &&invalid,
		[0xb3] = &&invalid, [0xb4] = &&invalid, [0xb5] = &&invalid,
		[0xb6] = &&invalid, [0xb7] = &&invalid, [0xb8] = &&invalid,
		[0xb9] = &&invalid, [0xba] = &&invalid, [0xbb] = &&invalid,
		[0xbc] = &&invalid, [0xbd] = &&invalid, [0xbe] = &&invalid,
		[0xbf] = &&invalid, [0xc0] = &&invalid, [0xc1] = &&invalid,
		[0xc2] = &&invalid, [0xc3] = &&invalid, [0xc4] = &&invalid,
		[0xc5] = &&invalid, [0xc6] = &&invalid, [0xc7] = &&invalid,
		[0xc8] = &&invalid, [0xc9] = &&invalid, [0xca] = &&invalid,
		[0xcb] = &&invalid, [0xcc] = &&invalid, [0xcd] = &&invalid,
		[0xce] = &&invalid, [0xcf] = &&invalid, [0xd0] = &&invalid,
		[0xd1] = &&invalid, [0xd2] = &&invalid, [0xd3] = &&invalid,
		[0xd4] = &&invalid, [0xd5] = &&invalid, [0xd6] = &&invalid,
		[0xd7] = &&invalid, [0xd8] = &&invalid, [0xd9] = &&invalid,
		[0xda] = &&invalid, [0xdb] = &&invalid, [0xdc] = &&invalid,
		[0xdd] = &&invalid, [0xde] = &&invalid, [0xdf] = &&invalid,
		[0xe0] = &&invalid, [0xe1] = &&invalid, [0xe2] = &&invalid,
		[0xe3] = &&invalid, [0xe4] = &&invalid, [0xe5] = &&invalid,
		[0xe6] = &&invalid, [0xe7] = &&invalid, [0xe8] = &&invalid,
		[0xe9] = &&invalid, [0xea] = &&invalid, [0xeb] = &&invalid,
		[0xec] = &&invalid, [0xed] = &&invalid, [0xee] = &&invalid,
		[0xef] = &&invalid, [0xf0] = &&invalid, [0xf1] = &&invalid,
		[0xf2] = &&invalid, [0xf3] = &&invalid, [0xf4] = &&invalid,
		[0xf5] = &&invalid, [0xf6] = &&invalid, [0xf7] = &&invalid,
		[0xf8] = &&invalid, [0xf9] = &&invalid, [0xfa] = &&invalid,
		[0xfb] = &&invalid, [0xfc] = &&invalid, [0xfd] = &&invalid,
		[0xfe] = &&invalid, [0xff] = &&invalid,
	};

	char c;
	struct kn_function_t *function;

start:
	goto *labels[(size_t) (c = PEEK())];

comment:
	while ((c = ADVANCE_PEEK()) != '\n') {
		if (c == '\0')
			goto expected_token;
	}
	// fallthrough, because we're currently a whitespace character (`\n`)

whitespace:
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
	//  then `strndup` it.
	const char *start = *stream;

	while (isident(ADVANCE_PEEK()));

	return kn_value_new_identifier(kn_string_new(
		strndup(start, *stream - start)));
}

string: {
	char quote = c;
	ADVANCE();
	const char *start = *stream;

	while ((c = PEEK_ADVANCE()) != quote) {
		if (c == '\0') {
			die("unterminated quote encountered: '%s'",
				start);
		}
	}

	size_t length = *stream - start - 1;
	struct kn_string_t *string;

	if (length == 0)
		string = &KN_STRING_EMPTY;
	else
		string = kn_string_new(strndup(start, length));

	return kn_value_new_string(string);
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
	struct kn_ast_t *ast = xmalloc(sizeof(struct kn_ast_t));

	ast->func = function;
	ast->refcount = 1;
	ast->args = xmalloc(sizeof(kn_value_t) * arity);

	for (size_t i = 0; i < arity; ++i)
		ast->args[i] = kn_parse(stream);

	return kn_value_new_ast(ast);
}


expected_token:
	die("unexpected end of stream");

invalid:
	die("unknown token start '%c'", c);
}
