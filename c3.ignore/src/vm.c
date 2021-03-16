#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "vm.h"
#include "shared.h"

struct _vm_t {
	const bytecode_t *code;
	unsigned ip, sp;
	value_t stack[];
};

vm_t *vm_new(const bytecode_t *code, unsigned length) {
	assert(length);

	vm_t *vm = xmalloc(sizeof(vm_t) + sizeof(value_t[STACKSIZE]));

	vm->code = code;
	vm->ip = length;
	vm->sp = 0;

	return vm;
}

inline static value_t *peek(vm_t *vm) {
	assert(vm->sp > 0);

	return &vm->stack[vm->sp - 1];
}

inline static void push(vm_t *vm, value_t value) {
	assert(vm->sp != STACKSIZE);

	vm->stack[vm->sp++] = value;
}

inline static value_t pop(vm_t *vm) {
	assert(vm->sp > 0);

	return vm->stack[--vm->sp];
}

inline static opcode_t next_instr(vm_t *vm) {
	assert(vm->ip > 0);

	return vm->code[--vm->ip].opcode;
}

inline static value_t next_value(vm_t *vm) {
	assert(vm->ip > 0);

	return vm->code[--vm->ip].value;
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

value_t vm_run(register vm_t *vm) {
	value_t args[MAX_ARGC];
	opcode_t op;

	while (vm->ip != 0) {
		switch (op = next_instr(vm)) {
		case OP_PUSHL:
			push(vm, next_value(vm));
			break;

		case OP_POP:
			pop(vm);
			break;

		case OP_PROMPT: die(0);
		case OP_RANDOM: die(0);
		case OP_EVAL: die(0);
		case OP_BLOCK: die(0);
		case OP_CALL: die(0);
		case OP_SYSTEM: die(0);
		case OP_QUIT: die(0);
		case OP_NOT: die(0);
		case OP_LENGTH: die(0);
		case OP_DUMP: {
			value_t tmp = RUN(*peek(vm));
			value_dump(tmp);
			*peek(vm) = tmp;
			break;
		}
		case OP_OUTPUT: die(0);
 
		case OP_ADD: {
			for (unsigned i = 0; i < OPCODE_ARGC(op); ++i)
				args[i] = pop(vm);
			args[0] = value_run(args[0]);


			if (ISSTR(args[0]))
				push(vm, NEWSTR(add_strings(ASSTR(args[0]), TOSTR(args[1]))));
			else {
				assert(ISNUM(args[0]));
				push(vm, NEWNUM(ASNUM(args[0]) + TONUM(args[1])));
			}
			break;
		}

		case OP_SUB:
			push(vm, NEWNUM(TONUM(pop(vm)) - TONUM(pop(vm))));
			break;

		case OP_MUL: die(0);
		case OP_DIV: die(0);
		case OP_MOD: die(0);
		case OP_POW: die(0);
		case OP_EQL: die(0);
		case OP_LTH: die(0);
		case OP_GTH: die(0);
		case OP_AND: die(0);
		case OP_OR: die(0);
		case OP_THEN: die(0);
		case OP_ASSIGN: die(0);
		case OP_WHILE: die(0);
		case OP_IF: die(0);
		case OP_GET: die(0);
		case OP_SET: die(0);

		default:
			die("invalid opcode '%d'", op);
		}
	}

	return pop(vm);
}
