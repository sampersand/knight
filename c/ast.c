#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "ast.h"
#include "shared.h"

static const unsigned ARITY_INCR = 0x20;

enum kn_token_kind {
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
	KN_TT_EQL,
	KN_TT_GTH,
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
};

#define MAX_ARITY (KN_TT_LAST__ / ARITY_INCR)

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
	case '(':
	case ')':
	case '[':
	case ']':
	case '{':
	case '}':
	case ':':
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


struct kn_ast_t kn_ast_parse_integer(stream_t stream) {
	kn_integer_t integer = 0;

	while (isdigit(peek(stream))) {
		integer = integer * 10 + next(stream) - '0';
	}

	return (struct kn_ast_t) {
		.kind = KN_TT_VALUE,
		.value = kn_value_new_integer(integer)
	};
}

struct kn_ast_t kn_ast_parse_identifier(stream_t stream) {
	size_t capacity = 8;
	size_t length = 0;
	char *identifier = xmalloc(capacity);
	char c;

	while (islower(c = peek(stream)) || isdigit(c) || c == '_') {
		identifier[length++] = c;
		advance(stream);

		if (length == capacity) {
			capacity *= 2;
			identifier = xrealloc(identifier, capacity);
		}
	}

	identifier[length++] = '\0';
	identifier = xrealloc(identifier, length);

	return (struct kn_ast_t) {
		.kind = KN_TT_IDENT,
		.ident = identifier
	};
}

struct kn_ast_t kn_ast_parse_string(stream_t stream) {
	size_t length = 0;
	size_t capacity = 1024;
	char *string = xmalloc(capacity);
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

	return (struct kn_ast_t) {
		.kind = KN_TT_VALUE,
		.value = kn_value_new_string(kn_string_new(string))
	};
}

struct kn_ast_t *kn_ast_parse_nonnull(stream_t);
struct kn_ast_t kn_ast_parse_keyword(stream_t stream) {
	char c = next(stream);
	struct kn_ast_t ast;

	switch (c) {
	// Symbol keywords: only parse a single char, then stop.
	case '`': ast.kind = KN_TT_SYS; goto parse_arguments;
	case '+': ast.kind = KN_TT_ADD; goto parse_arguments;
	case '-': ast.kind = KN_TT_SUB; goto parse_arguments;
	case '*': ast.kind = KN_TT_MUL; goto parse_arguments;
	case '/': ast.kind = KN_TT_DIV; goto parse_arguments;
	case '%': ast.kind = KN_TT_MOD; goto parse_arguments;
	case '^': ast.kind = KN_TT_POW; goto parse_arguments;
	case '<': ast.kind = KN_TT_LTH; goto parse_arguments;
	case '?': ast.kind = KN_TT_EQL; goto parse_arguments;
	case '>': ast.kind = KN_TT_GTH; goto parse_arguments;
	case ';': ast.kind = KN_TT_THEN; goto parse_arguments;
	case '&': ast.kind = KN_TT_AND; goto parse_arguments;
	case '|': ast.kind = KN_TT_OR; goto parse_arguments;
	case '=': ast.kind = KN_TT_ASSIGN; goto parse_arguments;

	// Word keywords: only the first upper case letter is a function.
	case 'T': ast.kind = KN_TT_TRUE; break;
	case 'F': ast.kind = KN_TT_FALSE; break;
	case 'N': ast.kind = KN_TT_NULL; break;
	case 'P': ast.kind = KN_TT_PROMPT; break;
	case 'E': ast.kind = KN_TT_EVAL; break;
	case 'B': ast.kind = KN_TT_BLOCK; break;
	case 'C': ast.kind = KN_TT_CALL; break;
	case 'Q': ast.kind = KN_TT_QUIT; break;
	case 'L': ast.kind = KN_TT_LENGTH; break;
	case 'G': ast.kind = KN_TT_GET; break;
	case 'S': ast.kind = KN_TT_SET; break;
	case 'O': ast.kind = KN_TT_OUPTUT; break;
	case 'W': ast.kind = KN_TT_WHILE; break;
	case 'R': ast.kind = KN_TT_RAND; break;
	case 'I': ast.kind = KN_TT_IF; break;

	default: die("unknown ast keyword start '%c'.", c);
	}

	// ignore remaining keyword symbols
	while (isupper(peek(stream))) {
		advance(stream);
	}

parse_arguments:

	for (int i = 0; i < arity(&ast); ++i) {
		ast.args[i] = kn_ast_parse_nonnull(stream);
	}

	if (ast.kind == KN_TT_ASSIGN && ast.args[0]->kind != KN_TT_IDENT) {
		die("attempted to assign to a non-identifier");
	}

	return ast;
}

struct kn_ast_t *kn_ast_parse(stream_t stream) {
	strip_stream(stream);
	char peeked = peek(stream);

	if (peeked == '\0') {
		return NULL;
	}

	struct kn_ast_t *ast = xmalloc(sizeof(struct kn_ast_t));

	if (isdigit(peeked)) {
		*ast = kn_ast_parse_integer(stream);
	} else if (islower(peeked) || peeked == '_') {
		*ast = kn_ast_parse_identifier(stream);
	} else if (peeked == '\'' || peeked == '"') {
		*ast = kn_ast_parse_string(stream);
	} else if (isupper(peeked) || ispunct(peeked)) {
		*ast = kn_ast_parse_keyword(stream);
	} else {
		die("unknown token start '%c'", peeked);
	}

	return ast;
}

struct kn_ast_t *kn_ast_parse_nonnull(stream_t stream) {
	struct kn_ast_t *ast = kn_ast_parse(stream);

	if (ast == NULL) {
		die("expected an expression");
	}

	return ast;
}

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

struct kn_value_t kn_ast_run(const struct kn_ast_t *ast) {
	struct kn_value_t ret;

	// only assign and block don't eval their parameters.
	switch (ast->kind) {
	case KN_TT_ASSIGN:
		ret = kn_ast_run(ast->args[1]);
		kn_env_set(ast->args[0]->ident, kn_value_clone(&ret));
		return ret;

	case KN_TT_BLOCK: {
		struct kn_ast_t *ptr = xmalloc(sizeof(struct kn_ast_t));
		ptr = kn_ast_clone(ast->args[0]);
		ret = kn_value_new_ast(ptr);
		return ret;
	}

	case KN_TT_WHILE: {
		ret = kn_value_new_null();
		struct kn_value_t condition = kn_ast_run(ast->args[0]);

		while (kn_value_to_boolean(&condition)) {
			kn_value_free(&condition);
			kn_value_free(&ret);
			ret = kn_ast_run(ast->args[1]);
			condition = kn_ast_run(ast->args[0]);
		}

		kn_value_free(&condition);
		return ret;
	}

	case KN_TT_IF: {
		struct kn_value_t condition = kn_ast_run(ast->args[0]);

		int which_arg = kn_value_to_boolean(&condition) ? 2 : 1;
		ret = kn_ast_run(ast->args[which_arg]);

		kn_value_free(&condition);
		return ret;
	}

	default:
		; // continue onwards
	}

	struct kn_value_t args[MAX_ARITY];
	for (int i = 0; i < arity(ast); ++i) {
		args[i] = kn_ast_run(ast->args[i]);
	}

	switch (ast->kind) {
	case KN_TT_VALUE:
		ret = kn_value_clone(&ast->value);
		break;

	case KN_TT_IDENT: {
		const struct kn_value_t *tmp = kn_env_get(ast->ident);

		if (tmp == NULL) {
			die("undefined identifier '%s' encountered.",
				ast->ident);
		}

		ret = kn_value_clone(tmp);
		break;
	}

	case KN_TT_PROMPT:{
		size_t linelen;
		char *nextline = fgetln(stdin, &linelen);

		if (nextline == NULL) {
			if (feof(stdin)) {
				ret = kn_value_new_string(kn_string_intern(""));
			} else {
				perror("unable to read line");
			}
		} else {
			ret = kn_value_new_string(kn_string_new(nextline));
		}
		break;
	}

	case KN_TT_RAND:
		ret = kn_value_new_integer((kn_integer_t) rand());
		break;

	case KN_TT_TRUE:
		ret = kn_value_new_boolean(1);
		break;

	case KN_TT_FALSE:
		ret = kn_value_new_boolean(0);
		break;

	case KN_TT_NULL:
		ret = kn_value_new_null();
		break;

	case KN_TT_EVAL: {
		struct kn_string_t string = kn_value_to_string(&args[0]);
		struct kn_ast_t *parsed_ast = kn_ast_parse(&string.str);

		if (parsed_ast == NULL) {
			ret = kn_value_new_null();
		} else {
			ret = kn_ast_run(parsed_ast);
			kn_ast_free(parsed_ast);
		}

		kn_string_free(&string);
		break;
	}

	case KN_TT_CALL:
		if (args[0].kind == KN_VT_AST) {
			ret = kn_ast_run(args[0].ast);
		} else {
			return args[0]; // return early 
		}

		break;

	case KN_TT_SYS: {
		struct kn_string_t command = kn_value_to_string(&args[0]);
		FILE *cmd_stream = popen(command.str, "r");

		if (cmd_stream == NULL) {
			die("unable to execute command");
		}

		char *result = "";
		die("todo: KN_TT_SYS");

		if (pclose(cmd_stream) == -1) {
			die("unable to close command stream");
		}

		kn_string_free(&command);
		ret = kn_value_new_string(kn_string_new(result));
		break;
	}

	case KN_TT_QUIT: {
		int exit_code = (int) kn_value_to_integer(&args[0]);
		exit(exit_code);
	}

	case KN_TT_NOT:
		ret = kn_value_new_boolean(!kn_value_to_boolean(&args[0]));
		break;

	case KN_TT_LENGTH: {
		struct kn_string_t string = kn_value_to_string(&args[0]);
	
		ret = kn_value_new_integer(strlen(string.str));
	
		kn_string_free(&string);
		break;
	}

	case KN_TT_OUPTUT: {
		struct kn_string_t string = kn_value_to_string(&args[0]);

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
		return args[0]; // we `return` so we don't free the arg.
	}


	case KN_TT_ADD:
		ret = kn_value_add(&args[0], &args[1]);
		break;

	case KN_TT_SUB:
		ret = kn_value_sub(&args[0], &args[1]);
		break;

	case KN_TT_MUL:
		ret = kn_value_mul(&args[0], &args[1]);
		break;

	case KN_TT_DIV:
		ret = kn_value_div(&args[0], &args[1]);
		break;

	case KN_TT_MOD:
		ret = kn_value_mod(&args[0], &args[1]);
		break;

	case KN_TT_POW:
		ret = kn_value_pow(&args[0], &args[1]);
		break;

	case KN_TT_LTH: 
	case KN_TT_EQL:
	case KN_TT_GTH: {
		int cmp = kn_value_cmp(&args[0], &args[1]);
		ret = kn_value_new_boolean(cmp == ast->kind - KN_TT_EQL);
		break;
	}

	case KN_TT_AND:
	case KN_TT_OR: {
		bool is_or = ast->kind - KN_TT_AND;

		// notably it runs both arguments.
		if (kn_value_to_boolean(&args[0])) {
			kn_value_free(&args[is_or]);
			return args[!is_or];
		} else {
			kn_value_free(&args[!is_or]);
			return args[is_or];
		}
	}

	case KN_TT_THEN:
		kn_value_free(&args[0]);
		return args[1]; // the return value is the RHS.

	case KN_TT_GET: {
		struct kn_string_t string = kn_value_to_string(&args[0]);
		size_t start = (size_t) kn_value_to_integer(&args[1]);
		size_t amnt = (size_t) kn_value_to_integer(&args[2]);
		size_t length = strlen(string.str);

		if (length - 1 <= start) {
			ret = kn_value_new_string(kn_string_intern(""));
		} else {
			char *substr = strndup(string.str + start, amnt);

			VERIFY_NOT_NULL(substr, "substring creation failed");

			ret = kn_value_new_string(kn_string_new(substr));
		}

		kn_string_free(&string);
	}

	case KN_TT_SET: {
		struct kn_string_t string = kn_value_to_string(&args[0]);
		size_t start = (size_t) kn_value_to_integer(&args[1]);
		size_t amnt = (size_t) kn_value_to_integer(&args[2]);
		size_t length = strlen(string.str);
		struct kn_string_t substr = kn_value_to_string(&args[3]);

		if (length - 1 <= start) {
			ret  = kn_value_new_string(string);
			goto after_freeing_string;
		}

		char *dup = strdup(string.str);
		dup += amnt;

		VERIFY_NOT_NULL(dup, "unable to duplicate string.");

		die("todo: set");

		ret = kn_value_new_string(kn_string_new(dup));

		kn_string_free(&string);
	after_freeing_string:
		kn_string_free(&substr);
		break;
	}

	case KN_TT_ASSIGN:
	case KN_TT_BLOCK:
	case KN_TT_WHILE:
	case KN_TT_IF:
		bug("function '%d' should have been handled.", ast->kind);

	default:
		bug("unknown function kind '%d'", ast->kind);
	}

	for (int i = 0; i < arity(ast); ++i) {
		kn_value_free(&args[i]);
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
