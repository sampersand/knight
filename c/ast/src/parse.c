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

// Used for functions which are only a single character, eg `+`.
#define SYMBOL_FUNC(name, sym) \
	KN_CGOTO_CASE(function_##name, sym): \
	function = &kn_fn_##name; \
	ADVANCE(); \
	goto parse_function

// Used for functions which are word functions (and can be multiple characters).
#define WORD_FUNC(name, sym) \
	KN_CGOTO_CASE(function_##name, sym): \
	function = &kn_fn_##name; \
	goto parse_kw_function

kn_value kn_parse(register const char **stream) {

// the global lookup table, which is used for the slightly-more-efficient, ut
// mnon-standard computed gotos version of the parser.
#ifdef KN_COMPUTED_GOTOS
	static const void *labels[256] = {
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

	KN_CGOTO_SWITCH(c, labels) {
	KN_CGOTO_CASE(comment, '#'):
		while (KN_LIKELY((c = ADVANCE_PEEK()) != '\n')) {

	#ifndef KN_RECKLESS
			// we hit end of stream, but we were looking for a token (as
			// otherwise we wouldn't be parsing values).
			if (KN_UNLIKELY(c == '\0'))
				goto expected_token;
	#endif /* !KN_RECKLESS */

		}

		assert(c == '\n');
		// fallthrough, because we're currently a whitespace character (`\n`)

	KN_CGOTO_CASE(whitespace,
		'\t', '\n', '\v', '\f', '\r', ' ',
		'(', ')', '[', ']', '{', '}', ':'
	):
		while (iswhitespace(ADVANCE_PEEK()));
		goto start; // go find the next token to return.

	KN_CGOTO_CASE(number, '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'):
	{
		kn_number number = (c - '0');

		while (isdigit(c = ADVANCE_PEEK()))
			number = number * 10 + (c - '0');

		return kn_value_new_number(number);
	}

	KN_CGOTO_LABEL(identifier)
	KN_CGOTO_CASES('a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j')
	KN_CGOTO_CASES('k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't')
	KN_CGOTO_CASES('u', 'v', 'w', 'x', 'y', 'z', '_')
	{	// simply find the start and end of the identifier
		const char *start = *stream;

		while (isident(ADVANCE_PEEK()));

		char *identifier = strndup(start, *stream - start);
		struct kn_variable *variable = kn_env_fetch(identifier, true);
		return kn_value_new_variable(variable);
	}

	KN_CGOTO_CASE(string, '\'', '\"'):
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

	KN_CGOTO_CASE(literal_true, 'T'):
		while(iswordfunc(ADVANCE_PEEK()));
		return KN_TRUE;

	KN_CGOTO_CASE(literal_false, 'F'):
		while(iswordfunc(ADVANCE_PEEK()));
		return KN_FALSE;

	KN_CGOTO_CASE(literal_null, 'N'):
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

#if defined(KN_COMPUTED_GOTOS) || !defined(KN_RECKLESS)
	expected_token:
#endif /* !KN_COMPUTED_GOTOS || !KN_RECKLESS */
	KN_CGOTO_CASES('\0')
		return KN_UNDEFINED;

	KN_CGOTO_DEFAULT(invalid):
		die("unknown token start '%c'", c);
	}

	KN_UNREACHABLE(); // every char should have an associated switch case.
}
