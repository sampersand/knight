#if 0
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include "value.h"
#include "parse.h"


/* get the first character of the stream. */
static char peek(const char **stream) {
	assert(stream != NULL);
	assert(*stream != NULL);
	return **stream;
}

/* advance a single character in the stream. */
static void advance(const char **stream) {
	assert(stream != NULL);
	assert(*stream != NULL);
	++(*stream);
}

/* advance a single character _and_ return the current one. */
static char next(const char **stream) {
	assert(stream != NULL);
	assert(*stream != NULL);
	return *((*stream)++);
}

static struct kn_value_t strip_whitespace(const char **stream) {
	assert(isspace(peek(stream)));

	do {
		advance(stream);
	} while (isspace(peek(stream)));

	return kn_value_parse(stream);
}

static struct kn_value_t strip_parens(const char **stream) {
#ifndef NDEBUG
	char c = peek(stream);
	assert(c == '(' || c == ')' || c == '[' || c == ']'
		|| c == '{' || c == '}' || c == ':');
#endif
	while (1) {
		switch (peek(stream)) {
		case '(':
		case ')':
		case '[':
		case ']':
		case '{':
		case '}':
		case ':':
			advance(stream);
			break;
		default:
			return kn_value_parse(stream);
		}
	}
}

static struct kn_value_t strip_comment(const char **stream) {
	assert(peek(stream) == '#');

	char c;

	do {
		c = next(stream);
	} while (c != '\0' && c != '\n');

	return kn_value_parse(stream);
}


static struct kn_value_t parse_integer(const char **stream) {
	assert(isdigit(peek(stream)));

	kn_integer_t integer = 0;

	// This may possibly overflow `integer`, but meh. Part of Knight's spec
	// is that implementations may define the max integer literal.
	do {
		integer *= 10;
		integer += next(stream) - '0';
	} while (isdigit(peek(stream)));

	return (struct kn_value_t) {
		.kind = KN_VK_INTEGER,
		.integer = integer
	};
}

static struct kn_value_t (*FUNCTIONS[256])(const char**) = {
	['\x00'] = expected_token,
	['\x01'] = invalid,
	['\x02'] = invalid,
	['\x03'] = invalid,
	['\x04'] = invalid,
	['\x05'] = invalid,
	['\x06'] = invalid,
	['\a']   = invalid,
	['\b']   = invalid,
	['\t']   = strip_whitespace,
	['\n']   = strip_whitespace,
	['\v']   = strip_whitespace,
	['\f']   = strip_whitespace,
	['\r']   = strip_whitespace,
	['\x0E'] = invalid,
	['\x0F'] = invalid,
	['\x10'] = invalid,
	['\x11'] = invalid,
	['\x12'] = invalid,
	['\x13'] = invalid,
	['\x14'] = invalid,
	['\x15'] = invalid,
	['\x16'] = invalid,
	['\x17'] = invalid,
	['\x18'] = invalid,
	['\x19'] = invalid,
	['\x1A'] = invalid,
	['\e']   = invalid,
	['\x1C'] = invalid,
	['\x1D'] = invalid,
	['\x1E'] = invalid,
	['\x1F'] = invalid,
	[' ']    = strip_whitespace,
	['!']    = function_not,
	['"']    = string,
	['#']    = comment,
	['$']    = invalid,
	['%']    = function_mod,
	['&']    = function_and,
	['\'']   = string,
	['(']    = strip_parens,
	[')']    = strip_parens,
	['*']    = function_mul,
	['+']    = function_add,
	[',']    = invalid,
	['-']    = function_sub,
	['.']    = invalid,
	['/']    = function_div,
	['0']    = integer,
	['1']    = integer,
	['2']    = integer,
	['3']    = integer,
	['4']    = integer,
	['5']    = integer,
	['6']    = integer,
	['7']    = integer,
	['8']    = integer,
	['9']    = integer,
	[':']    = strip_parens,
	[';']    = function_then,
	['<']    = function_lth,
	['=']    = function_assign,
	['>']    = function_gth,
	['?']    = function_eql,
	['@']    = invalid,
	['A']    = invalid,
	['B']    = function_block,
	['C']    = function_call,
	['D']    = function_debug,
	['E']    = function_eval,
	['F']    = literal_false,
	['G']    = function_get,
	['H']    = invalid,
	['I']    = function_if,
	['J']    = invalid,
	['K']    = invalid,
	['L']    = function_length,
	['M']    = invalid,
	['N']    = literal_null,
	['O']    = function_output,
	['P']    = function_prompt,
	['Q']    = function_quit,
	['R']    = function_random,
	['S']    = function_set,
	['T']    = literal_true,
	['U']    = invalid,
	['V']    = invalid,
	['W']    = function_while,
	['X']    = invalid,
	['Y']    = invalid,
	['Z']    = invalid,
	['[']    = strip_parens,
	['\\']   = invalid,
	[']']    = strip_parens,
	['^']    = function_pow,
	['_']    = identifier,
	['`']    = function_system,
	['a']    = identifier,
	['b']    = identifier,
	['c']    = identifier,
	['d']    = identifier,
	['e']    = identifier,
	['f']    = identifier,
	['g']    = identifier,
	['h']    = identifier,
	['i']    = identifier,
	['j']    = identifier,
	['k']    = identifier,
	['l']    = identifier,
	['m']    = identifier,
	['n']    = identifier,
	['o']    = identifier,
	['p']    = identifier,
	['q']    = identifier,
	['r']    = identifier,
	['s']    = identifier,
	['t']    = identifier,
	['u']    = identifier,
	['v']    = identifier,
	['w']    = identifier,
	['x']    = identifier,
	['y']    = identifier,
	['z']    = identifier,
	['{']    = strip_parens,
	['|']    = function_or,
	['}']    = strip_parens,
	['~']    = invalid,
	['\x7f'] = invalid,
	['\x80'] = invalid, ['\x81'] = invalid, ['\x82'] = invalid,
	['\x83'] = invalid, ['\x84'] = invalid, ['\x85'] = invalid,
	['\x86'] = invalid, ['\x87'] = invalid, ['\x88'] = invalid,
	['\x89'] = invalid, ['\x8a'] = invalid, ['\x8b'] = invalid,
	['\x8c'] = invalid, ['\x8d'] = invalid, ['\x8e'] = invalid,
	['\x8f'] = invalid, ['\x90'] = invalid, ['\x91'] = invalid,
	['\x92'] = invalid, ['\x93'] = invalid, ['\x94'] = invalid,
	['\x95'] = invalid, ['\x96'] = invalid, ['\x97'] = invalid,
	['\x98'] = invalid, ['\x99'] = invalid, ['\x9a'] = invalid,
	['\x9b'] = invalid, ['\x9c'] = invalid, ['\x9d'] = invalid,
	['\x9e'] = invalid, ['\x9f'] = invalid, ['\xa0'] = invalid,
	['\xa1'] = invalid, ['\xa2'] = invalid, ['\xa3'] = invalid,
	['\xa4'] = invalid, ['\xa5'] = invalid, ['\xa6'] = invalid,
	['\xa7'] = invalid, ['\xa8'] = invalid, ['\xa9'] = invalid,
	['\xaa'] = invalid, ['\xab'] = invalid, ['\xac'] = invalid,
	['\xad'] = invalid, ['\xae'] = invalid, ['\xaf'] = invalid,
	['\xb0'] = invalid, ['\xb1'] = invalid, ['\xb2'] = invalid,
	['\xb3'] = invalid, ['\xb4'] = invalid, ['\xb5'] = invalid,
	['\xb6'] = invalid, ['\xb7'] = invalid, ['\xb8'] = invalid,
	['\xb9'] = invalid, ['\xba'] = invalid, ['\xbb'] = invalid,
	['\xbc'] = invalid, ['\xbd'] = invalid, ['\xbe'] = invalid,
	['\xbf'] = invalid, ['\xc0'] = invalid, ['\xc1'] = invalid,
	['\xc2'] = invalid, ['\xc3'] = invalid, ['\xc4'] = invalid,
	['\xc5'] = invalid, ['\xc6'] = invalid, ['\xc7'] = invalid,
	['\xc8'] = invalid, ['\xc9'] = invalid, ['\xca'] = invalid,
	['\xcb'] = invalid, ['\xcc'] = invalid, ['\xcd'] = invalid,
	['\xce'] = invalid, ['\xcf'] = invalid, ['\xd0'] = invalid,
	['\xd1'] = invalid, ['\xd2'] = invalid, ['\xd3'] = invalid,
	['\xd4'] = invalid, ['\xd5'] = invalid, ['\xd6'] = invalid,
	['\xd7'] = invalid, ['\xd8'] = invalid, ['\xd9'] = invalid,
	['\xda'] = invalid, ['\xdb'] = invalid, ['\xdc'] = invalid,
	['\xdd'] = invalid, ['\xde'] = invalid, ['\xdf'] = invalid,
	['\xe0'] = invalid, ['\xe1'] = invalid, ['\xe2'] = invalid,
	['\xe3'] = invalid, ['\xe4'] = invalid, ['\xe5'] = invalid,
	['\xe6'] = invalid, ['\xe7'] = invalid, ['\xe8'] = invalid,
	['\xe9'] = invalid, ['\xea'] = invalid, ['\xeb'] = invalid,
	['\xec'] = invalid, ['\xed'] = invalid, ['\xee'] = invalid,
	['\xef'] = invalid, ['\xf0'] = invalid, ['\xf1'] = invalid,
	['\xf2'] = invalid, ['\xf3'] = invalid, ['\xf4'] = invalid,
	['\xf5'] = invalid, ['\xf6'] = invalid, ['\xf7'] = invalid,
	['\xf8'] = invalid, ['\xf9'] = invalid, ['\xfa'] = invalid,
	['\xfb'] = invalid, ['\xfc'] = invalid, ['\xfd'] = invalid,
	['\xfe'] = invalid, ['\xff'] = invalid,
};

struct kn_value_t kn_value_parse(const char **stream) {
	return FUNCTIONS[(size_t) peek(stream)](stream);
};
#endif

int _();
