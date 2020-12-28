#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "ast.h"
#include "shared.h"

/*
 * The kinds of tokens that exist within Knight.
 *
 * Technically, there's only three "real" token types: values, identifiers, and
 * functions. However, for ease of use, each function is defined to be its own
 * token as well.
 *
 * The `value` and `ident` fields are exclusively used by the `KN_TT_VALUE` and
 * `KN_TT_IDENT` fields, respectively. The `args` field is used by the function
 * tokens. Arguments start from `args[0]` and count upwards to one less than the
 * arity of the function; arguments beyond that are undefined.
 *
 * With the sole exception of `KN_TT_EQL`, every function will automatically
 * convert types to the types they expect.
 */
enum kn_token_kind {

/******** Arity Zero *********/
	/*
	 * A literal value---ie a `kn_value_t`.
	 */
	KN_TT_VALUE = 'v',

	/*
	 * An identifier within Knight.
	 */
	KN_TT_IDENT = 'i',

	/*
	 * `TRUE`:  The "TRUE" function; this just returns a `true` `kn_value_t`.
	 */
	KN_TT_TRUE = 'T',

	/*
	 * `FALSE`: The "FALSE" function; this just returns a `false` `kn_value_t`.
	 */
	KN_TT_FALSE = 'F',

	/*
	 * `NULL`: The "NULL" function; this just returns a `null` `kn_value_t`.
	 */
	KN_TT_NULL = 'N',

	/*
	 * `PROMPT`: Reads a single line from stdin and returns it.
	 *
	 * If stdin is at EOF, an empty string is returned.
	 */
	KN_TT_PROMPT = 'P',

	/*
	 * `RAND`: Returns a random `kn_integer_t`.
	 */
	KN_TT_RAND = 'R',

/******** Arity One *********/

	/*
	 * `EVAL str`: Evaluates `str` as if it were Knight code.
	 *
	 * If an only-whitespace/comments string is given, `null` is returned.
	 */
	KN_TT_EVAL = 'E',

	/*
	 * `BLOCK arg`: Returns `arg`, unevaluated.
	 *
	 * This is used to delay execution; blocks can be executed via the
	 * function `KN_TT_CALL`.
	 */
	KN_TT_BLOCK = 'B',

	/*
	 * `CALL block`: Executes a block of code that was created via
	 * `KN_TT_BLOCK`, returning that expression's result.
	 *
	 * If given a non-block value, it aborts execution.
	 */
	KN_TT_CALL = 'C',

	/*
	 * `` `(str) ``: Executes `str` as a system command, returning the
	 * stdout of that function.
	 *
	 * If there are problems during the opening of the command, a warning
	 * is logged, and `null` is returned.
	 */
	KN_TT_SYS = '`',

	/*
	 * `QUIT code`: Halts execution with the status code `code`.
	 */
	KN_TT_QUIT = 'Q', 

	/*
	 * `! arg`: Converts `arg` to a boolean, then returns the logical
	 * negation of it.
	 */
	KN_TT_NOT = '!',

	/*
	 * `LENGTH str`: Returns the length of `str`.
	 */
	KN_TT_LENGTH = 'L',

	/*
	 * `OUTPUT msg`: Outputs `msg` as a string, followed by a newline.
	 *
	 * If `msg` ends with a backslash (`\`), then the newline is suppressed.
	 */
	KN_TT_OUPTUT = 'O',

/******** Arity Two *********/
	/*
	 * `WHILE cond body`: Executes the `body` continuously until `cond`
	 * becomes falsey.
	 *
	 * Returns the last result of the body, or `null` if the body wasn't
	 * executed at all.
	 */
	KN_TT_WHILE = 'W',

	/*
	 * `+ lhs rhs`: If `lhs` is a string, `rhs` is converted to a string
	 * and concatenated with `lhs`. Otherwise, both are converted to numbers
	 * and added together.
	 */
	KN_TT_ADD = '+',

	/*
	 * `- lhs rhs`: Converts both arguments to integers and subtracts
	 * `rhs` from `lhs`.
	 */
	KN_TT_SUB = '-',

	/*
	 * `* lhs rhs`: If `lhs` is a string, then `lhs` concatenated with
	 * itself `rhs` times is returned. Otherwise, both are converted to an
	 * integer and multiplied together.
	 */
	KN_TT_MUL = '*',

	/*
	 * `/ lhs rhs`: Converts both arguments to integers and divides `lhs`
	 * by `rhs`.
	 *
	 * Execution is aborted if the second argument is zero.
	 */
	KN_TT_DIV = '/',

	/*
	 * `% lhs rhs`: Converts both arguments to integers and modulos `lhs`
	 * by `rhs`.
	 *
	 * Execution is aborted if the second argument is zero.
	 */
	KN_TT_MOD = '%',

	/*
	 * `^ lhs rhs`: Raises `lhs` to the power of `rhs`. Returns `1` if
	 * `rhs` is zero.
	 */
	KN_TT_POW = '^',

	/*
	 * `< lhs rhs`: If `lhs` is a string, checks to see if `lhs` is less
	 * than `rhs` lexicographically. Othewise, both are converted to numbers 
	 * and then compared.
	 */
	KN_TT_LTH = '<',

	/*
	 * `> lhs rhs`: If `lhs` is a string, checks to see if `lhs` is less
	 * than `rhs` lexicographically. Othewise, both are converted to numbers 
	 * and then compared.
	 */
	KN_TT_GTH = '>',

	/*
	 * `? lhs rhs`: Returns `true` only if both values are the same kind, and
	 * are the same value for that type.
	 *
	 * NOTE: This is the only function that doesn't do automatic conversion
	 * of arguments. To do conversions, you can use `& < l r > l r`.
	 */
	KN_TT_EQL = '?',

	/*
	 * `; lhs rhs`: Executes `lhs`, then `rhs` is executed, returning
	 * `rhs`'s result.
	 */
	KN_TT_THEN = ';',

	/*
	 * `& lhs rhs`: Executes `lhs`. If that's falsey, it's returned 
	 * directly; otherwise, `rhs` is executed and its result is returned.
	 */
	KN_TT_AND = '&',

	/*
	 * `| lhs rhs`: Executes `lhs`. If that's truthy, it's returned 
	 * directly; otherwise, `rhs` is executed and its result is returned.
	 */
	KN_TT_OR = '|',

	/*
	 * `= ident arg`: Assigns the global variable `ident` to `arg`.
	 *
	 * Execution is aborted if the first argument isn't an identifier.
	 */
	KN_TT_ASSIGN = '=',
	
/******** Arity Three *********/
	/*
	 * `I cond if_t if_f`: If `cond` is truthy, `if_t` is executed and
	 * returned. Otherwise, `if_f` is executed and returned.
	 */
	KN_TT_IF = 'I',

	/*
	 * `GET str idx len`: Gets a substring from `str`, starting at `idx` and 
	 * is at most `len` characters long. (It can be less if `str + idx` is
	 * past the end of the array)
	 *
	 * If `idx` is greater than `str`'s length, an empty string is returned.
	 */
	KN_TT_GET = 'G',

/******** Arity Four *********/
	/*
	 * `SET str idx len repl`: Returns a copy of `str`, with the substring
	 * starting at `idx`, and at most `len` characters long, replaced with
	 * `repl`.
	 *
	 * If `idx` is greater than `str`'s length, the original string is
	 * returned.
	 */
	KN_TT_SET = 'S',
};

#define ARITY_INCR /*todo*/ 1
#define MAX_ARITY /*todo*/ 5

struct kn_ast_t {
	enum kn_token_kind kind;

	union {
		struct kn_value_t value;
		const char *ident;
		struct kn_ast_t *args[MAX_ARITY];
	};
};

unsigned arity(const struct kn_ast_t *ast) {
	return ast->kind / ARITY_INCR;
}

#include "ast/parse.c"
#include "ast/run.c"

struct kn_ast_t *kn_ast_clone(const struct kn_ast_t *ast) {
	struct kn_ast_t *ret = xmalloc(sizeof(struct kn_ast_t));
	ret->kind = ast->kind;

	if (ret->kind == KN_TT_VALUE) {
		ret->value = kn_value_clone(&ast->value);
	} else if (ret->kind == KN_TT_IDENT) {
		char *ident = strdup((char *) ast->ident);

		VERIFY_NOT_NULL(ident, "unable to duplicate an identifier");

		ret->ident = ident;
	} else {
		for (int i = 0; i < arity(ret); ++i)  {
			ret->args[i] = kn_ast_clone(ast->args[i]);
		}
	}
	return ret;
}

void kn_ast_free(struct kn_ast_t *ast) {
	for (int i = 0; i < arity(ast); ++i) {
		kn_ast_free(ast->args[i]);
	}

	if (ast->kind == KN_TT_VALUE) {
		kn_value_free(&ast->value);
	} else if (ast->kind == KN_TT_IDENT) {
		xfree((void *) ast->ident);
	}
}

void kn_ast_dump_indent(const struct kn_ast_t *ast, int indent) {
	switch(ast->kind) {
	case KN_TT_VALUE: {
		struct kn_string_t string = kn_value_to_string(&ast->value);
		printf("v:%s\n", string.str);
		kn_string_free(&string);
		break;
	}

	case KN_TT_IDENT: printf("%s\n", ast->ident); break;
	case KN_TT_PROMPT: printf("PROMPT\n"); break;
	case KN_TT_TRUE: printf("TRUE\n"); break;
	case KN_TT_FALSE: printf("FALSE\n"); break;
	case KN_TT_NULL: printf("NULL\n"); break;
	case KN_TT_EVAL: printf("EVAL\n"); break;
	case KN_TT_BLOCK: printf("BLOCK\n"); break;
	case KN_TT_CALL: printf("CALL\n"); break;
	case KN_TT_SYS: printf("SYS\n"); break;
	case KN_TT_QUIT: printf("QUIT\n"); break;
	case KN_TT_NOT: printf("NOT\n"); break;
	case KN_TT_LENGTH: printf("LENGTH\n"); break;
	case KN_TT_GET: printf("GET\n"); break;
	case KN_TT_SET: printf("SET\n"); break;
	case KN_TT_OUPTUT: printf("OUPTUT\n"); break;
	case KN_TT_WHILE: printf("WHILE\n"); break;
	case KN_TT_RAND: printf("RANDOM\n"); break;
	case KN_TT_ADD: printf("ADD\n"); break;
	case KN_TT_SUB: printf("SUB\n"); break;
	case KN_TT_MUL: printf("MUL\n"); break;
	case KN_TT_DIV: printf("DIV\n"); break;
	case KN_TT_MOD: printf("MOD\n"); break;
	case KN_TT_POW: printf("POW\n"); break;
	case KN_TT_LTH: printf("LTH\n"); break;
	case KN_TT_GTH: printf("GTH\n"); break;
	case KN_TT_EQL: printf("EQL\n"); break;
	case KN_TT_THEN: printf("THEN\n"); break;
	case KN_TT_AND: printf("AND\n"); break;
	case KN_TT_OR: printf("OR\n"); break;
	case KN_TT_ASSIGN: printf("ASSIGN\n"); break;
	case KN_TT_IF: printf("IF\n"); break;
	default:
		bug("unknown kind '%d'", ast->kind);
	}

	++indent;

	for(int i = 0; i < arity(ast); ++i) {
		for(int i = 0; i < indent; ++i)
			printf("| ");
		kn_ast_dump_indent(ast->args[i], indent);
	}

}

void kn_ast_dump(const struct kn_ast_t *ast) {
	kn_ast_dump_indent(ast, 0);
}
