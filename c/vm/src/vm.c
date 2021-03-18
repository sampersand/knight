#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "vm.h"
#include "shared.h"
#include "env.h"
#include "string.h"
#include "knight.h"

#define PUSH(value) (assert(sp != STACKSIZE), stack[sp++] = (value))
#define POP() (assert(sp != 0), stack[--sp])
#define NEXT_INSTR() (assert(ip != block->length), block->code[ip++].opcode)
#define NEXT_VALUE() (assert(ip != block->length), block->code[ip++].value)

static string_t *mul_strings(string_t *lhs, size_t length) {
	(void) lhs;
	(void) length;

	die(0);
}

static string_t *add_strings(string_t *lhs, string_t *rhs) {
	if (lhs->length == 0) {
		string_free(lhs);
		return rhs;
	}

	if (rhs->length == 0) {
		string_free(rhs);
		return lhs;
	}

	size_t length = lhs->length + rhs->length;
	char *str = xmalloc(length + 1);

	memcpy(str, lhs->str, lhs->length);
	memcpy(str + lhs->length, rhs->str, rhs->length);
	str[length] = '\0';

	string_free(lhs);
	string_free(rhs);

	return string_emplace(str, length);
}

#define RUN(n) (value_run(n))
#define FREE(n) (value_free(n))

#define NEWNUM(n) (value_new_number(n))
#define NEWSTR(n) (value_new_string(n))
#define NEWBOOL(n) (value_new_boolean(n))

#define ASNUM(n) (value_as_number(n))
#define ASSTR(n) (value_as_string(n))
#define ASBOOL(n) (value_as_boolean(n))

#define TONUM(n) (value_to_number(n))
#define TOSTR(n) (value_to_string(n))
#define TOBOOL(n) (value_to_boolean(n))

#define ISNUM(n) (value_is_number(n))
#define ISSTR(n) (value_is_string(n))

#define arg0 (args[0])
#define arg1 (args[1])
#define arg2 (args[2])
#define arg3 (args[3])

static value_t stack[STACKSIZE];
static unsigned sp = 0;

void run_block(blockptr_t *blockptr) {
	block_t *block = blockptr->block;
	unsigned ip = blockptr->ip;

	value_t args[MAX_ARGC];
	opcode_t op;

	while (ip != block->length) {
		op = NEXT_INSTR();

		for (unsigned i = 0; i < OPCODE_ARGC(op); ++i) {
			printf("%x\n", op);
			args[OPCODE_ARGC(op) - i - 1] = POP();
		}

		switch (op) {
		case OP_PUSHL:
			PUSH(NEXT_VALUE());
			break;

		case OP_PROMPT: {
			size_t cap = 0;
			ssize_t len;
			char *line = NULL;

			if ((len = getline(&line, &cap, stdin)) == -1) {
				assert(line != NULL);
				free(line);

				if (!feof(stdin)) 
					perror("unable to read line");

				PUSH(NEWSTR(&STRING_EMPTY));
				break;
			}

			assert(0 <= len);
			assert(line != NULL);

			char *ret = strndup(line, len);
			free(line);

			PUSH(NEWSTR(string_emplace(ret, len)));

			break;
		}

		case OP_RANDOM:
			PUSH(NEWNUM(rand()));
			break;

		case OP_POP:
			// do nothing, we popped already
			break;

		case OP_EVAL: {
			string_t *string = TOSTR(arg0);
			PUSH(execute(string->str));
			free(string);
			break;
		}

		case OP_CALL:
			if (value_is_variable(arg0))
				PUSH(value_clone(value_as_variable(arg0)->value));
			else if (value_is_block(arg0))
				run_block(value_as_block(arg0));
			else
				PUSH(value_clone(arg0));

			break;

		case OP_SYSTEM: {
			string_t *command = TOSTR(arg0);
			FILE *stream = popen(command->str, "r");

			if (stream == NULL)
				die("unable to execute command '%s'.", command->str);

			string_free(command);

			size_t tmp;
			size_t cap = 2048;
			size_t len = 0;
			char *result = xmalloc(cap);

			// try to read the entire stream's stdout to `result`.
			while (0 != (tmp = fread(result + len, 1, cap - len, stream))) {
				len += tmp;

				if (len == cap) {
					cap *= 2;
					result = xrealloc(result, cap);
				}
			}

			if (ferror(stream)) die("unable to read command stream");

			result = xrealloc(result, len + 1);
			result[len] = '\0';

			if (pclose(stream) == -1) die("unable to close command stream.");	

			PUSH(NEWSTR(string_emplace(result, len)));
		}

		case OP_QUIT:
			exit(TONUM(arg0));

		case OP_NOT:
			PUSH(NEWBOOL(!TOBOOL(arg0)));
			break;

		case OP_LENGTH: {
			string_t *string = TOSTR(arg0);
			PUSH(TONUM(string->length));
			string_free(string);
			break;
		}

		case OP_DUMP:
			arg0 = RUN(arg0);
			value_dump(arg0);
			PUSH(arg0);
			break;

		case OP_OUTPUT: {
			string_t *string = TOSTR(arg0);

			if (string->length && string->str[string->length - 1] == '\\') {
				string->str[string->length - 1] = '\0';
				fputs(string->str, stdout);
				string->str[string->length - 1] = '\\';
			} else
				puts(string->str);

			string_free(string);
			PUSH(NULL_);
			break;
		}
 
		case OP_ADD:
			arg0 = RUN(arg0);

			if (ISSTR(arg0))
				PUSH(NEWSTR(add_strings(ASSTR(arg0), TOSTR(arg1))));
			else
				PUSH(NEWNUM(ASNUM(arg0) + TONUM(arg1)));
			break;

		case OP_SUB:
			PUSH(NEWNUM(TONUM(arg0) - TONUM(arg1)));
			break;

		case OP_MUL:
			if (ISSTR(arg0))
				PUSH(NEWSTR(mul_strings(ASSTR(arg0), TONUM(arg1))));
			else
				PUSH(NEWNUM(ASNUM(arg0) * TONUM(arg1)));
			break;

		case OP_DIV:
			PUSH(NEWNUM(TONUM(arg0) / TONUM(arg1)));
			break;

		case OP_MOD:
			PUSH(NEWNUM(TONUM(arg0) % TONUM(arg1)));
			break;

		case OP_POW:
			assert(0 && "todo: pow");

		case OP_EQL:
			arg0 = RUN(arg0);
			arg1 = RUN(arg1);

			if (arg0 == arg1)
				PUSH(NEWBOOL(1));
			else if (!ISSTR(arg0) || !ISSTR(arg1))
				PUSH(NEWBOOL(0));
			else
				PUSH(NEWBOOL(!strcmp(ASSTR(arg0)->str, ASSTR(arg1)->str)));

			FREE(arg0);
			FREE(arg1);
			break;


		case OP_GTH:
			// swap operands
			arg2 = arg0;
			arg0 = arg1;
			arg1 = arg2;

			// fallthrough.
		case OP_LTH:
			arg0 = RUN(arg0);

			if (ISSTR(arg0)) {
				string_t *lhs = ASSTR(arg0), *rhs = TOSTR(arg1);

				PUSH(strcmp(lhs->str, rhs->str) < 0);
				string_free(lhs);
				string_free(rhs);
			} else if (ISNUM(arg0))
				PUSH(NEWBOOL(ASNUM(arg0) < TONUM(arg1)));
			else
				PUSH(ASBOOL(arg0) < TOBOOL(arg1));
			break;

		case OP_AND:
			arg0 = RUN(arg0);

			if (TOBOOL(arg0)) {
				PUSH(RUN(arg1));
				FREE(arg0);
			} else
				PUSH(arg0);
			break;

		case OP_OR:
			arg0 = RUN(arg0);

			if (TOBOOL(arg0))
				PUSH(arg0);
			else {
				PUSH(RUN(arg1));
				FREE(arg0);
			}
			break;

		case OP_THEN:
			FREE(RUN(arg0));
			PUSH(RUN(arg1));
			break;

		case OP_ASSIGN:
			arg1 = RUN(arg1);
			value_as_variable(arg0)->value = value_clone(arg1);
			PUSH(arg1);
			break;

		case OP_WHILE:
			while (RUN(arg0))
				FREE(RUN(arg1));
			PUSH(NULL_);
			break;

		case OP_IF:
			PUSH(RUN(TOBOOL(arg0) ? arg1 : arg2));
			break;

		case OP_GET: {
			string_t *string = TOSTR(arg0);
			size_t start = TONUM(arg1), length = TONUM(arg2);

			if (string->length <= start) {
				string_free(string);
				PUSH(NEWSTR(&STRING_EMPTY));
				break;
			}

			if (string->length <= start + length)
				length = string->length - start;

			char *substr = xmalloc(length);

			memcpy(substr, string->str + start, length);
			substr[length] = '\0';
			PUSH(NEWSTR(string_emplace(substr, length)));

			string_free(string);
			break;
		}

		case OP_SET: {
			string_t *string, *substr;
			size_t start, amnt, length;

			string = TOSTR(arg0);
			start = TONUM(arg1);
			amnt = TONUM(arg2);
			substr = TOSTR(arg3);

			if (string->length < start) 
				die("index '%zu' out of bounds (length=%zu)", start,
					string->length);

			if (string->length <= start + amnt)
				amnt = string->length - start;

			length = string->length - amnt + substr->length;

			char *str = xmalloc(length + 1);
			str[length] = '\0';

			char *ptr = str;

			memcpy(ptr, string->str, start);
			ptr += start;
			memcpy(ptr, substr->str, substr->length);
			ptr += substr->length;
			memcpy(ptr, string->str + start + amnt, string->length - amnt);

			string_free(string);
			string_free(substr);

			PUSH(NEWSTR(string_emplace(str, length)));
			break;
		}

		default:
			die("invalid opcode '%d'", op);
		}
	}
}

value_t vm_run(block_t *block) {
	blockptr_t blockptr = { .block = block, .ip = 0, .rc = 0 };
	run_block(&blockptr);
	return POP();
}

