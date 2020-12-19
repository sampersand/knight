#include "ast.h"
#include "shared.h"
#include <ctype.h>
#include <string.h>

static const unsigned ARITY_INCR = 0x20;

typedef enum {
	// arity zero
	KN_TT_VALUE = 0x00,
	KN_TT_IDENT,
	KN_TT_PROMPT,

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
	KN_TT_GET = 2 * ARITY_INCR,
	KN_TT_SET,
	KN_TT_WHILE,
	KN_TT_RANDOM,
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
	KN_TT_AOR,
	KN_TT_ASSIGN,
	
	// arity three
	KN_TT_IF = 3 * ARITY_INCR,
	KN_TT_LAST_
} kn_token_kind;

#define ARITY_(kind) ((kind) / ARITY_INCR)

static const unsigned MAX_ARITY = ARITY_(KN_TT_LAST_);

typedef struct kn_ast_t {
	kn_token_kind kind;
	union {
		kn_value_t value;
		const char *ident;
		kn_ast_t *args[MAX_ARITY];
	};
} kn_ast_t;

unsigned arity(const kn_ast_t *ast) {
	return ARITY_(ast->kind);
}

typedef const char ** stream_t;

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
		case '|': ast->kind = KN_TT_AOR; break;
		case '=': ast->kind = KN_TT_ASSIGN; break;
		default: die("unknown keyword start '%c'", c);
	}

	if(arity(ast) == 0)
		return ast;

	ast->args[0] = kn_ast_parse(stream);
	if (arity(ast) == 1)
		return ast;

	ast->args[1] = kn_ast_parse(stream);

	if(ast->kind == KN_TT_ASSIGN && ast->args[0]->kind != KN_TT_IDENT) {
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
		case 'R': ast->kind = KN_TT_RANDOM; break;
		case 'I': ast->kind = KN_TT_IF; break;
		default: die("unknown ast keyword start '%c'.", c);
	}

	while (isupper(peek(stream)))
		advance(stream);

	ast->args[0] = kn_ast_parse(stream);
	if (arity(ast) == 1)
		return ast;

	ast->args[1] = kn_ast_parse(stream);
	if (arity(ast) == 2)
		return ast;

	ast->args[2] = kn_ast_parse(stream);
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

kn_value_t kn_ast_run(const kn_ast_t *ast) {
	const kn_value_t *ret;

	switch(ast->kind) {
		case KN_TT_VALUE:
			ret = &ast->value;
			goto done;
		case KN_TT_IDENT:
			if (NULL == (ret = kn_env_get(ast->ident))) {
				die("unknown identifier '%s' encountered", ast->ident);
			}
			goto done;
		case KN_TT_PROMPT:
			die("todo: KN_TT_PROMPT");
		case KN_TT_ASSIGN:
			ret = kn_env_set(ast->args[0]->ident, kn_ast_run(ast->args[1]));
			goto done;
		default:
			;
			// onwards!
	}

	// if (ast->kind)
	die("Todo");

		// KN_TT_EVAL = ARITY_INCR,
		// KN_TT_BLOCK, 
		// KN_TT_CALL,
		// KN_TT_SYS,
		// KN_TT_QUIT, 
		// KN_TT_NOT,
		// KN_TT_LENGTH,
		// KN_TT_OUPTUT,

		// // arity two
		// KN_TT_GET = 2 * ARITY_INCR,
		// KN_TT_SET,
		// KN_TT_WHILE,
		// KN_TT_RANDOM,
		// KN_TT_ADD,
		// KN_TT_SUB,
		// KN_TT_MUL,
		// KN_TT_DIV,
		// KN_TT_MOD,
		// KN_TT_POW,
		// KN_TT_LTH,
		// KN_TT_GTH,
		// KN_TT_EQL,
		// KN_TT_THEN,
		// KN_TT_AND,
		// KN_TT_AOR,
		// KN_TT_ASSIGN,
		
		// // arity three
		// KN_TT_IF = 3 * ARITY_INCR,
	done:
		return kn_value_clone(ret);
}

void kn_ast_free(kn_ast_t *ast) {
	switch (arity(ast)) {
		case 3:
			kn_ast_free(ast->args[2]);
		case 2:
			kn_ast_free(ast->args[1]);
		case 1:
			kn_ast_free(ast->args[0]);
			return;
		case 0:
			switch (ast->kind) {
				case KN_TT_VALUE:
					kn_value_free(&ast->value);
					return;
				case KN_TT_IDENT:
					xfree((void *) ast->ident);
					// fallthru
				case KN_TT_PROMPT:
					return;
				default:
					bug("unknown nullary ast kind: %d", ast->kind);
			}
		default:
			bug("unknown ast arity: %d", arity(ast));
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
		case KN_TT_RANDOM: printf("RANDOM\n"); break;
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
		case KN_TT_AOR: printf("AOR\n"); break;
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
