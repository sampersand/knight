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

void run(kn_vm_t *vm) {
	while (1) {
		switch (ninstr(vm)) {
		case OP_SUB:
			// push(vm, pop(vm).number - pop(vm).number);
			break;
		default:
			return;
		}
	}
}
