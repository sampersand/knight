#include "bytecode.h"
#include "shared.h"
#include "env.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>

void block_free(block_t *block) {
	if (!--block->rc)
		free(block);
}

void block_clone(block_t *block) {
	++block->rc;
}

int isupper_orunder(char c) {
	return isupper(c) || c == '_';
}

int isspace_orparen(char c) {
	return isspace(c) || c == ':' || c == '(' || c == ')' || c == '[' ||
		c == ']' || c == '{' || c == '}';
}

static void block_resize(block_t *block) {
	block->capacity *= 2;
	block->code = xrealloc(block->code, sizeof(value_t[block->capacity]));
}

static void block_push_value(block_t *block, value_t value) {
	if (block->length >= block->capacity - 1)
		block_resize(block);
	block->code[block->length++].opcode = OP_PUSHL;
	block->code[block->length++].value = value;
}

static void block_push_opcode(block_t *block, opcode_t opcode) {
	if (block->length >= block->capacity)
		block_resize(block);

	block->code[block->length++].opcode = opcode;
}

static void block_parse_inner(const char **stream, block_t *block) {
	opcode_t op;

	switch (**stream) {
		case '\0':
		expected_value:
			die("expected a value");

		case '#':
			while (*(*stream)++ != '\n')
				if (**stream == '\0') goto expected_value;
			// fallthrough
		case ' ': case '\n': case '\t': case '\r': case '\f':
		case '[': case ']': case '{': case '}': case '(': case ')': case ':':
			while (isspace_orparen(**stream)) ++*stream;
			block_parse_inner(stream, block);
			return;

		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9': {
			number_t number = 0;

			do {
				number *= 10;
				number += **stream - '0';
				++*stream;
			} while (isdigit(**stream));

			block_push_value(block, value_new_number(number));
			return;
		}

		case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
		case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
		case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u':
		case 'v': case 'w': case 'x': case 'y': case 'z': case '_': {
			const char *start = *stream;

			do {
				++*stream;
			} while (isdigit(**stream) || islower(**stream) || **stream == '_');

			block_push_value(block, value_new_variable(
				env_fetch(strndup(start, *stream - start), 1)));
			return;
		}

		case '\"': case '\'': {
			char quote = **stream;
			++*stream;
			const char *start = *stream;

			while (**stream != quote) {
				if (**stream == '\0') die("unterminated quote found");
				++*stream;
			}

			size_t length = *stream - start;
			++*stream;

			block_push_value(block, value_new_string(
				string_emplace(strndup(start, length), length)));
			return;
		}

		case 'N':
			block_push_value(block, NULL_);
			goto stripliteral;
		case 'F':
			block_push_value(block, FALSE_);
			goto stripliteral;
		case 'T':
			block_push_value(block, TRUE_);
		stripliteral:
			while(isupper_orunder(*++*stream));
			return;

		case 'B': {
			while (isupper_orunder(**stream)) ++*stream;
			blockptr_t *blockptr = xmalloc(sizeof(blockptr_t));
			blockptr->block = block;
			blockptr->rc = 1;
			blockptr->ip = block->length + 1; // the starting position.
			block_push_value(block, value_new_block(blockptr));
			return;
		}

		case 'C': op = OP_CALL; goto parse_kw_function;
		case 'D': op = OP_DUMP; goto parse_kw_function;
		case 'E': op = OP_EVAL; goto parse_kw_function;
		case 'G': op = OP_GET; goto parse_kw_function;
		case 'I': op = OP_IF; goto parse_kw_function;
		case 'L': op = OP_LENGTH; goto parse_kw_function;
		case 'O': op = OP_OUTPUT; goto parse_kw_function;
		case 'P': op = OP_PROMPT; goto parse_kw_function;
		case 'R': op = OP_RANDOM; goto parse_kw_function;
		case 'S': op = OP_SET; goto parse_kw_function;
		case 'W': op = OP_WHILE; goto parse_kw_function;

		case '`': op = OP_SYSTEM; goto parse_function;
		case '!': op = OP_NOT; goto parse_function;
		case '%': op = OP_MOD; goto parse_function;
		case '^': op = OP_POW; goto parse_function;
		case '&': op = OP_AND; goto parse_function;
		case '*': op = OP_MUL; goto parse_function;
		case '+': op = OP_ADD; goto parse_function;
		case '-': op = OP_SUB; goto parse_function;
		case '=': op = OP_ASSIGN; goto parse_function;
		case '|': op = OP_OR; goto parse_function;
		case ';': op = OP_THEN; goto parse_function;
		case '<': op = OP_LTH; goto parse_function;
		case '>': op = OP_GTH; goto parse_function;
		case '?': op = OP_EQL; goto parse_function;

		default: die("unknown token start '%1$c' (0x%1$02x)", **stream & 0xff);
	}

	parse_kw_function:
		while (isupper_orunder(**stream)) ++*stream;
		goto parsefn;
	parse_function:
		++*stream;
	parsefn:
		if (op != OP_THEN) // `then` is implicit.
			block_push_opcode(block, op);

		for (unsigned i = 0; i < OPCODE_ARGC(op); ++i)
			block_parse_inner(stream, block);

}

block_t *block_parse(const char **stream) {
	block_t *block = xmalloc(sizeof(block_t));

	block->length = 0;
	block->capacity = 128;
	block->code = xmalloc(sizeof(value_t[block->capacity]));
	block_parse_inner(stream, block);
	block->rc = 0;

	return block;
}

void block_dump(const block_t *block) {
	opcode_t op;

	for (unsigned i = 0; i < block->length; ++i) {
		op = block->code[i].opcode;

		printf("[% 3d:%u] (%02x) ", i, OPCODE_ARGC(op), op);

		switch (op) {
			case OP_UNDEFINED: printf("OP_UNDEFINED"); break;
			case OP_PROMPT: printf("OP_PROMPT"); break;
			case OP_RANDOM: printf("OP_RANDOM"); break;
			case OP_PUSHL:
				printf("OP_PUSHL: ");
				value_dump(block->code[++i].value);
				printf("\n");
				continue;

			case OP_EVAL: printf("OP_EVAL"); break;

			case OP_CALL: printf("OP_CALL"); break;
			case OP_SYSTEM: printf("OP_SYSTEM"); break;
			case OP_QUIT: printf("OP_QUIT"); break;
			case OP_NOT: printf("OP_NOT"); break;
			case OP_LENGTH: printf("OP_LENGTH"); break;
			case OP_DUMP: printf("OP_DUMP"); break;
			case OP_OUTPUT: printf("OP_OUTPUT"); break;
			case OP_POP: printf("OP_POP"); break;

			case OP_ADD: printf("OP_ADD"); break;
			case OP_SUB: printf("OP_SUB"); break;
			case OP_MUL: printf("OP_MUL"); break;
			case OP_DIV: printf("OP_DIV"); break;
			case OP_MOD: printf("OP_MOD"); break;
			case OP_POW: printf("OP_POW"); break;
			case OP_EQL: printf("OP_EQL"); break;
			case OP_LTH: printf("OP_LTH"); break;
			case OP_GTH: printf("OP_GTH"); break;
			case OP_AND: printf("OP_AND"); break;
			case OP_OR: printf("OP_OR"); break;
			case OP_THEN: printf("OP_THEN"); break;
			case OP_ASSIGN: printf("OP_ASSIGN"); break;
			case OP_WHILE: printf("OP_WHILE"); break;

			case OP_IF: printf("OP_IF"); break;
			case OP_GET: printf("OP_GET"); break;

			case OP_SET: printf("OP_SET"); break;
		}
		printf(" \n");
	}
}
