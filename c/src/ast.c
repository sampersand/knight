#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "ast.h"
#include "shared.h"
#include "function.h"

// The "stream" type.
typedef const char **stream_t;

static char peek(stream_t stream) {
	return **stream;
}

static void advance(stream_t stream) {
	++(*stream);
}

static char next(stream_t stream) {
	return *((*stream)++);
}

static bool is_eof(stream_t stream) {
	return peek(stream) == '\0';
}

static void strip_stream(stream_t stream) {
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
		while (!is_eof(stream) && next(stream) != '\n') {
			// do nothing until we hit EOF or EOL.
		}

		strip_stream(stream);
		return;

		// fall through, as will be immediately after a comment.
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
		integer *= 10;
		integer += next(stream) - '0';
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

struct kn_ast_t kn_ast_parse_keyword(stream_t stream) {
	char name = peek(stream);
	const struct kn_function_t *function = kn_fn_fetch(name);

	if (function == NULL) {
		die("unknown function '%c' encountered.", name);
	}

	// advance stream.
	do {
		advance(stream);
	} while (isupper(name) && isupper(peek(stream)));

	size_t arity = function->arity;

	struct kn_ast_t ast = (struct kn_ast_t) {
		.kind = KN_TT_FUNCTION,
		.function = function,
		.args = xmalloc(arity * sizeof(struct kn_ast_t))
	};

	for (size_t i = 0; i < arity; ++i) {
		ast.args[i] = kn_ast_parse(stream);
	}

	return ast;
}

struct kn_ast_t kn_ast_parse(stream_t stream) {
	strip_stream(stream);
	char peeked = peek(stream);

	if (isdigit(peeked)) {
		return kn_ast_parse_integer(stream);
	} else if (islower(peeked) || peeked == '_') {
		return kn_ast_parse_identifier(stream);
	} else if (peeked == '\'' || peeked == '"') {
		return kn_ast_parse_string(stream);
	} else if (isupper(peeked) || ispunct(peeked)) {
		return kn_ast_parse_keyword(stream);
	} else if (peeked == '\0') {
		die("unexpected eof; expected an expression.");
	} else {
		die("unknown token start '%c'", peeked);
	}
}

struct kn_value_t kn_ast_run(const struct kn_ast_t *ast) {
	switch (ast->kind) {
	case KN_TT_VALUE:
		return kn_value_clone(&ast->value);

	case KN_TT_IDENT: {
		const struct kn_value_t *retptr = kn_env_get(ast->ident);

		if (retptr == NULL) {
			die("unknown identifier '%s'", ast->ident);
		}

		return kn_value_clone(retptr);
	}

	case KN_TT_FUNCTION:
		return (ast->function->func)(ast->args);

	default:
		bug("unknown kind '%d'");
	}
}

struct kn_ast_t kn_ast_clone(const struct kn_ast_t *ast) {
	struct kn_ast_t ret = (struct kn_ast_t) {
		.kind = ast->kind
	};

	switch (ret.kind) {
	case KN_TT_VALUE:
		ret.value = kn_value_clone(&ast->value);
		break;

	case KN_TT_IDENT: {
		char *ident = strdup((char *) ast->ident);

		VERIFY_NOT_NULL(ident, "unable to duplicate an identifier");

		ret.ident = ident;
		break;
	}

	case KN_TT_FUNCTION: {
		ret.function = ast->function;
		size_t arity = ret.function->arity;

		ret.args = xmalloc(arity * sizeof(struct kn_ast_t));

		for (size_t i = 0; i < arity; i++) {
			ret.args[i] = kn_ast_clone(&ast->args[i]);
		}

		break;
	}

	default:
		bug("unknown kind '%d'", ast->kind);
	}

	return ret;
}

void kn_ast_free(struct kn_ast_t *ast) {
	switch (ast->kind) {
	case KN_TT_VALUE:
		kn_value_free(&ast->value);
		break;

	case KN_TT_IDENT:
		xfree((void *) ast->ident);
		break;

	case KN_TT_FUNCTION:
		for (size_t i = 0; i < ast->function->arity; ++i) {
			kn_ast_free(&ast->args[i]);
		}

		xfree((void *) ast->args);
		break;
	default:
		bug("unknown kind '%d'", ast->kind);
	}
}
