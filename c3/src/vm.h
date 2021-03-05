#ifndef VM_H
#define VM_H

#include "value.h"
#include "bytecode.h"

#define STACKSIZE 1000

typedef struct {
	kn_bytecode_t *code;
	kn_value_t *stack;
	int ip;
	int sp;
} kn_vm_t;

kn_vm_t *new_vm(kn_bytecode_t *code);
void run(kn_vm_t *vm);

#endif
