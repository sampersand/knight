#include "ast.h"
#include "shared.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

static const unsigned ARITY_INCR = 0x20;

typedef enum {
	// arity zero
	KN_TT_VALUE = 0x00,
	KN_TT_IDENT,
	KN_TT_TRUE,
	KN_TT_FALSE,
	KN_TT_NULL,
	KN_TT_PROMPT,
	KN_TT_RAND,

	// arity one
	KN_TT_EVAL = ARITY_INCR,
	KN_TT_BLOCK, 
	KN_TT_CALL,
	KN_TT_SYS,
	KN_TT_QUIT, 
	KN_TT_NOT,
	KN_TT_LENGTH,
	KN_TT_OUPTUT,

	// arity two
	KN_TT_WHILE = 2 * ARITY_INCR,
	KN_TT_ADD,
	KN_TT_SUB,
	KN_TT_MUL,
	KN_TT_DIV,
	KN_TT_MOD,
	KN_TT_POW,
	KN_TT_LTH,
	KN_TT_GTH,
	KN_TT_EQL,
	KN_TT_THEN,
	KN_TT_AND,
	KN_TT_OR,
	KN_TT_ASSIGN,
	
	// arity three
	KN_TT_IF = 3 * ARITY_INCR,
	KN_TT_GET,

	// arity four
	KN_TT_SET = 4 * ARITY_INCR,
	KN_TT_LAST__
} kn_token_kind;

typedef struct kn_ast_t {
	kn_token_kind kind;
	union {
		kn_value_t value;
		const char *ident;
		kn_ast_t *args[KN_TT_LAST__ / ARITY_INCR];
	};
} kn_ast_t;

unsigned arity(const kn_ast_t *ast) {
	return ast->kind / ARITY_INCR;
}

char peek(stream_t stream) {
	return **stream;
}

void advance(stream_t stream) {
	++(*stream);
}

char next(stream_t stream) {
	return *((*stream)++);
}

bool is_eof(stream_t stream) {
	return peek(stream) == '\0';
}

void strip_stream(stream_t stream) {
	char c = peek(stream);
	switch (c) {
		case '(': case ')':
		case '[': case ']':
		case '{': case '}':
		case ':':
			advance(stream);
			break;
		case '#':
			while (!is_eof(stream) && peek(stream) != '\n') {
				advance(stream);
			}
			// fall through, as we'll either be at EOF or `\n`.
		default:
			if (!isspace(c)) {
				return;
			}
	}

	advance(stream);
	strip_stream(stream);
}


kn_ast_t *kn_ast_parse_integer(stream_t stream) {
	kn_integer_t integer = 0;

	while(isdigit(peek(stream))) {
		integer = integer * 10 + next(stream) - '0';
	}

	kn_ast_t *ast = xmalloc(sizeof(kn_ast_t));
	ast->kind = KN_TT_VALUE;
	ast->value = kn_value_new_integer(integer);

	return ast;
}

kn_ast_t *kn_ast_parse_identifier(const char **stream) {
	static char identifier[1024];
	size_t length = 0;
	char c;

	while (islower(c = peek(stream)) || isdigit(c) || c == '_') {
		identifier[length++] = c;
		advance(stream);

		if (length == sizeof(identifier)) {
			die("Too large of an identifier encountered: %s", identifier);
		}
	}

	identifier[length] = '\0';

	kn_ast_t *ast = xmalloc(sizeof(kn_ast_t));
	ast->kind = KN_TT_IDENT;
	ast->ident = strdup(identifier);

	return ast;
}

kn_ast_t *kn_ast_parse_string(const char **stream) {
	size_t length = 0;
	size_t capacity = 1024;
	char *string = xmalloc(capacity * sizeof(char));
	char quote = next(stream);
	char c;

	while(!is_eof(stream) && (c = next(stream)) != quote) {
		string[length++] = c;

		if (length == capacity) {
			capacity *= 2;
			string = xrealloc(string, capacity);
		}
	}

	string[length++] = '\0';

	if (c != quote) {
		die("unterminated quote encountered: %c%s\n", quote, string);
	}


	string = xrealloc(string, length); // remove unused capacity
	kn_ast_t *ast = xmalloc(sizeof(kn_ast_t));
	ast->kind = KN_TT_VALUE;
	ast->value = kn_value_new_string(xrealloc(string, length));

	return ast;
}

kn_ast_t *kn_ast_parse_keyword_symbol(stream_t stream) {
	char c = next(stream);
	kn_ast_t *ast = xmalloc(sizeof(kn_ast_t));

	switch (c) {
		case '`': ast->kind = KN_TT_SYS; break;
		case '+': ast->kind = KN_TT_ADD; break;
		case '-': ast->kind = KN_TT_SUB; break;
		case '*': ast->kind = KN_TT_MUL; break;
		case '/': ast->kind = KN_TT_DIV; break;
		case '%': ast->kind = KN_TT_MOD; break;
		case '^': ast->kind = KN_TT_POW; break;
		case '<': ast->kind = KN_TT_LTH; break;
		case '>': ast->kind = KN_TT_GTH; break;
		case '?': ast->kind = KN_TT_EQL; break;
		case ';': ast->kind = KN_TT_THEN; break;
		case '&': ast->kind = KN_TT_AND; break;
		case '|': ast->kind = KN_TT_OR; break;
		case '=': ast->kind = KN_TT_ASSIGN; break;
		default: die("unknown keyword start '%c'", c);
	}

	if(arity(ast) == 0)
		return ast;

	ast->args[0] = kn_ast_parse(stream);
	if (arity(ast) == 1)
		return ast;

	ast->args[1] = kn_ast_parse(stream);

	if (ast->kind == KN_TT_ASSIGN && ast->args[0]->kind != KN_TT_IDENT) {
		kn_ast_dump(ast->args[0]);
		die("attempted to assign to a non-identifier");
	}

	if (arity(ast) == 2)
		return ast;

	ast->args[2] = kn_ast_parse(stream);
	return ast;
}

kn_ast_t *kn_ast_parse_keyword(stream_t stream) {
	char c = next(stream);
	kn_ast_t *ast = xmalloc(sizeof(kn_ast_t));
	switch (c) {
		case 'T': ast->kind = KN_TT_TRUE; break;
		case 'F': ast->kind = KN_TT_FALSE; break;
		case 'N': ast->kind = KN_TT_NULL; break;
		case 'P': ast->kind = KN_TT_PROMPT; break;
		case 'E': ast->kind = KN_TT_EVAL; break;
		case 'B': ast->kind = KN_TT_BLOCK; break;
		case 'C': ast->kind = KN_TT_CALL; break;
		case 'Q': ast->kind = KN_TT_QUIT; break;
		case 'L': ast->kind = KN_TT_LENGTH; break;
		case 'G': ast->kind = KN_TT_GET; break;
		case 'S': ast->kind = KN_TT_SET; break;
		case 'O': ast->kind = KN_TT_OUPTUT; break;
		case 'W': ast->kind = KN_TT_WHILE; break;
		case 'R': ast->kind = KN_TT_RAND; break;
		case 'I': ast->kind = KN_TT_IF; break;
		default: die("unknown ast keyword start '%c'.", c);
	}

	while (isupper(peek(stream)))
		advance(stream);

	for (int i = 0; i < arity(ast); ++i) {
		ast->args[i] = kn_ast_parse(stream);
	}

	return ast;
}

kn_ast_t *kn_ast_parse(stream_t stream) {
	strip_stream(stream);
	char peeked = peek(stream);

	if (peeked == '\0') {
		return NULL;
	} else if (isdigit(peeked)) {
		return kn_ast_parse_integer(stream);
	} else if (islower(peeked) || peeked == '_') {
		return kn_ast_parse_identifier(stream);
	} else if (peeked == '\'' || peeked == '"') {
		return kn_ast_parse_string(stream);
	} else if (isupper(peeked)) {
		return kn_ast_parse_keyword(stream);
	} else if (ispunct(peeked)) {
		return kn_ast_parse_keyword_symbol(stream);
	} else {
		die("unknown token start '%c'", peeked);
	}
}

kn_value_t kn_ast_run_arity_0(const kn_ast_t *ast) {
	switch(ast->kind) {
		case KN_TT_VALUE:
			return kn_value_clone(&ast->value);

		case KN_TT_IDENT:
			{
				const kn_value_t *ret = kn_env_get(ast->ident);

				if (ret == NULL) {
					die("unknown identifier '%s' encountered", ast->ident);
				}

				return kn_value_clone(ret);
			}

		case KN_TT_PROMPT:
			die("todo: prompt");

		case KN_TT_RAND:
			return kn_value_new_integer((kn_integer_t) rand());

		case KN_TT_TRUE:
			return kn_value_new_boolean(1);

		case KN_TT_FALSE:
			return kn_value_new_boolean(0);

		case KN_TT_NULL:
			return kn_value_new_null();

		default:
			bug("unknown nullary function '%d'", ast->kind);
	}
}

kn_value_t kn_ast_run_arity_1(const kn_ast_t *ast) {
	kn_value_t arg1 = kn_ast_run(ast->args[0]);
	kn_value_t ret;

	switch(ast->kind) {
		case KN_TT_EVAL:
			{
				kn_string_t string_to_parse = kn_value_to_string(&arg1);
				kn_ast_t *parsed_ast = kn_ast_parse((stream_t) &string_to_parse);

				if (parsed_ast == NULL) {
					ret = kn_value_new_null();
				} else {
					ret = kn_ast_run(parsed_ast);
					kn_ast_free(parsed_ast);
				}

				kn_string_free((kn_ast_t *) string_to_parse);
			}

		case KN_TT_CALL:
			if (arg1.kind != KN_VT_AST) {
				die("cannot call non-ast type '%d'", arg1.kind);
			}

			ret = kn_ast_run(arg1.ast);
			break;

		case KN_TT_SYS:
			die("todo: KN_TT_SYS");

		case KN_TT_QUIT:
			exit((int) kn_value_to_integer(&arg1));

		case KN_TT_NOT:
			ret = kn_value_new_boolean(!kn_value_to_boolean(&arg1));
			break;

		case KN_TT_LENGTH:
			{
				kn_string_t string = kn_value_to_string(&arg1);
				ret = kn_value_new_integer(strlen(string));
				kn_string_free(string);
			}
			break;

		case KN_TT_OUPTUT:
			{
				kn_string_t string = kn_value_to_string(&arg1);
				size_t len = strlen(string);

				if (len != 0 && string[len - 2] != '\\') {
					printf("%s\n", string);
				} else {
					string[len - 2] = '\0';
					printf("%s", string);
				}

				kn_string_free(string);
			}

			return arg1; // we `return` so we don't `free` it.

		case KN_TT_BLOCK:
			bug("KN_TT_BLOCK not meant to be called from kn_ast_run_arity_1");

		default:
			bug("unknown unary operator '%d'", ast->kind);
	}

	kn_value_free(&arg1);
	return ret;
}

kn_value_t kn_ast_run_arity_2(const kn_ast_t *ast) {
	kn_value_t arg1 = kn_ast_run(ast->args[0]);
	kn_value_t arg2 = kn_ast_run(ast->args[1]);
	kn_value_t ret;

	switch(ast->kind) {
		case KN_TT_ADD:
			if (arg1.kind == KN_VT_STRING) {
				kn_string_t rhs = kn_value_to_string(&arg2);
				char *allocated = xmalloc(strlen(arg1.string) + strlen(rhs) + 1);

				strcpy(allocated, arg1.string);
				strcat(allocated, rhs);

				xfree(rhs);
				ret = kn_value_new_string(allocated);
			} else {
				ret = kn_value_new_integer(kn_value_to_integer(&arg1) + kn_value_to_integer(&arg2));
			}
			break;

		case KN_TT_SUB:
			ret = kn_value_new_integer(kn_value_to_integer(&arg1) - kn_value_to_integer(&arg2));
			break;

		case KN_TT_MUL:
			ret = kn_value_new_integer(kn_value_to_integer(&arg1) * kn_value_to_integer(&arg2));
			break;

		case KN_TT_DIV:
			ret = kn_value_new_integer(kn_value_to_integer(&arg1) / kn_value_to_integer(&arg2));
			break;

		case KN_TT_MOD:
			ret = kn_value_new_integer(kn_value_to_integer(&arg1) % kn_value_to_integer(&arg2));
			break;

		case KN_TT_POW:
			ret = kn_value_pow(&arg1, &arg2);
			break;

		case KN_TT_LTH: 
			ret = kn_value_new_boolean(kn_value_cmp(&arg1, &arg2) == -1);
			break;

		case KN_TT_GTH: 
			ret = kn_value_new_boolean(kn_value_cmp(&arg1, &arg2) == 1);
			break;

		case KN_TT_EQL: 
			ret = kn_value_new_boolean(kn_value_cmp(&arg1, &arg2) == 0);
			break;

		case KN_TT_AND:
			// notably it runs both arguments.
			ret = kn_value_to_boolean(&arg1) ? arg2 : arg1;
			break;

		case KN_TT_AND:
			// notably it runs both arguments.
			ret = kn_value_to_boolean(&arg1) ? arg1 : arg2;
			break;

		case KN_TT_THEN:
			kn_value_free(&arg1);
			return arg2; // the return value is the RHS.

		case KN_TT_ASSIGN:
			bug("KN_TT_WHILE not meant to be called from kn_ast_run_arity_2");
		case KN_TT_WHILE:
			bug("KN_TT_WHILE not meant to be called from kn_ast_run_arity_2");
		default:
			bug("unknown binary operator '%d'", ast->kind);


	}

	kn_value_free(&arg1);
	kn_value_free(&arg2);
	return ret;
}

kn_value_t kn_ast_run_arity_3(const kn_ast_t *ast) {
	die("");
}

kn_ast_t kn_ast_clone(const kn_ast_t *ast) {
	die("Todo colone ast");
}

kn_value_t kn_ast_run(const kn_ast_t *ast) {
	kn_value_t ret;

	// only assign and block don't eval their parameters.
	switch (ast->kind) {
		case KN_TT_ASSIGN:
			ret = kn_ast_run(ast->args[1]);
			kn_env_set(ast->args[0]->ident, kn_value_clone(&ret));
			break;

		case KN_TT_BLOCK:
			{
				kn_ast_t *ptr = xmalloc(sizeof(kn_ast_t));
				*ptr = kn_ast_clone(ast->args[0]);
				ret = kn_value_new_ast(ptr);
			}

			break;

		case KN_TT_WHILE:
			ret = kn_value_new_null();
			{
				kn_value_t condition = kn_ast_run(ast->args[0]);

				while (kn_value_to_boolean(&condition)) {
					kn_value_free(&condition);
					kn_value_free(&ret);
					ret = kn_ast_run(ast->args[1]);
					condition	= kn_ast_run(ast->args[0]);
				}

				kn_value_free(&condition);
			}
			break;

		case KN_TT_IF:
			{
				kn_value_t condition = kn_ast_run(ast->args[0]);
				ret = kn_ast_run(ast->args[kn_value_to_boolean(&condition) ? 2 : 1]);
				kn_value_free(&condition);
			}
			break;

		default:
			switch(arity(ast)) {
				case 0:
					ret = kn_ast_run_arity_0(ast);
					break;

				case 1:
					ret = kn_ast_run_arity_1(ast);
					break;

				case 2:
					ret = kn_ast_run_arity_2(ast);
					break;

				case 3:
					ret = kn_ast_run_arity_3(ast);
					break;

				default:
					bug("unknown arity '%d'", arity(ast));
			}
	}

	return ret;

}

void kn_ast_free(kn_ast_t *ast) {
	for (int i = 0; i < arity(ast); ++i) {
		kn_ast_free(ast->args[i]);
	}

	if (ast->kind == KN_TT_VALUE) {
		kn_value_free(&ast->value);
	} else if (ast->kind == KN_TT_IDENT) {
		xfree((void *) ast->ident);
	}
}

void kn_ast_dump_indent(const kn_ast_t *ast, int indent) {
	kn_string_t string;
	switch(ast->kind) {
		case KN_TT_VALUE:
			string = kn_value_to_string(&ast->value);
			printf("v:%s\n", string);
			xfree(string);
			break;
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

	if (arity(ast) == 0)
		return;
	++indent;

	for(int i = 0; i < indent; ++i)
		printf("| ");
	kn_ast_dump_indent(ast->args[0], indent);

	if (arity(ast) == 1)
		return;
	for(int i = 0; i < indent; ++i)
		printf("| ");
	// printf("%d", ast);
	kn_ast_dump_indent(ast->args[1], indent);

	if (arity(ast) == 2)
		return;
	for(int i = 0; i < indent; ++i)
		printf("| ");
	kn_ast_dump_indent(ast->args[2], indent);

}

void kn_ast_dump(const kn_ast_t *ast) {
	kn_ast_dump_indent(ast, 0);
}
