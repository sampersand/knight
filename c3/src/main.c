#include "vm.h"
#include "knight.h"
#include <stdio.h>

int main() {
	kn_initialize();

	bytecode_t bytecode[] = {
		{ OP_DUMP  },
		{ OP_ADD   },
		{ .value = value_new_string(string_new("12foobar")) }, { OP_PUSHL },
		{ .value = value_new_number(1) },  { OP_PUSHL },
		// { value_new_number(10) }, { OP_PUSHL },
	};

	vm_t *vm = vm_new(&*bytecode, 6);
	value_free(vm_run(vm));
}
