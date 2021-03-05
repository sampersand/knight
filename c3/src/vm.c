#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "vm.h"
#include "shared.h"

kn_vm_t *new_vm(kn_bytecode_t *code) {
	kn_vm_t *vm = xmalloc(sizeof(kn_vm_t));

	vm->stack = xmalloc(sizeof(kn_value_t[STACKSIZE]));
	vm->code = code;
	vm->ip = 0;
	vm->sp = 0;

	return vm;
}

static void push(kn_vm_t *vm, kn_value_t value) {
	assert(vm->sp != STACKSIZE);

	vm->stack[vm->sp++] = value;
}

static kn_value_t pop(kn_vm_t *vm) {
	assert(vm->sp != 0);

	return vm->stack[vm->sp--];
}

static kn_opcode_t ninstr(kn_vm_t *vm) {
	return vm->code[vm->ip++].opcode;
}

static kn_string_t *add_strings(kn_string_t *lhs, kn_string_t *rhs) {
	if (lhs->length == 0) {
		kn_string_free(lhs);
		return rhs;
	}

	if (rhs->length == 0) {
		kn_string_free(rhs);
		return lhs;
	}

	size_t length = lhs->length + rhs->length;
	char *str = xmalloc(length + 1);

	memcpy(str, lhs->str, lhs->length);
	memcpy(str + lhs->length, rhs->str, rhs->length);
	str[length] = '\0';

	kn_string_free(lhs);
	kn_string_free(rhs);

	return kn_string_emplace(str, length);
}

kn_value_t run(kn_vm_t *vm) {
	kn_value_t args[KN_MAX_ARGC];
	kn_value_t ret;

	while (1) {
		kn_opcode_t op = ninstr(vm);

		for (unsigned i = 0; i < KN_OPCODE_ARGC(op); ++i)
			args[i] = pop(vm);

		switch (op) {
		case KN_OP_ADD:
			args[0] = kn_value_run(args[0]);

			if (kn_value_is_string(args[0]))
				ret = kn_value_new_string(add_strings(
					kn_value_as_string(args[0]),
					kn_value_to_string(args[1])));
			else
				ret = kn_value_new_number(
					kn_value_as_number(args[0])
					+ kn_value_to_number(args[0]));
			push(vm, ret);
			break;
		case KN_OP_SUB:
			// push(vm, pop(vm).number - pop(vm).number);
			break;
		default:
			return 0;
		}
	}
}
