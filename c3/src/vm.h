#ifndef VM_H
#define VM_H

#include "value.h"
#include "bytecode.h"

#define STACKSIZE 10000

typedef struct _vm_t vm_t;

vm_t *vm_new(const bytecode_t *, unsigned);
value_t vm_run(vm_t *);

#endif
