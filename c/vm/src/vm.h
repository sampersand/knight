#ifndef VM_H
#define VM_H

#include "value.h"
#include "bytecode.h"

#define STACKSIZE 10000

value_t vm_run(block_t *);

#endif
