#include "vm.h"
#include "knight.h"
#include <stdio.h>

int main(int argc, char **argv) {
	initialize();

	execute(argc == 1 ? "DUMP+3 4" : argv[1]);

	// static block_t block = {
	// 	.rc = 0, .length = 6,
	// 	.code = {
	// 		{ OP_PUSHL }, { 0 },
	// 		{ OP_PUSHL }, { 0 },
	// 		{ OP_SUB   },
	// 		{ OP_DUMP  },
	// 	}
	// 	// { value_new_number(10) }, { OP_PUSHL },
	// };
	// block.code[1] = (bytecode_t) { .value = value_new_string(string_new("12foobar")) };
	// block.code[3] = (bytecode_t) { .value = value_new_number(1) };

	// vm_t *vm = vm_new(&*bytecode, 6);
	// value_free(vm_run(&block));
}
