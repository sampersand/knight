
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
		if (peek(stream) == '\n') {
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
	// printf("[%s]\n", *stream);
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
