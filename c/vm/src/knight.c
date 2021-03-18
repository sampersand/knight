#include "knight.h"
#include "bytecode.h"
#include "vm.h"
#include "env.h"
#include <stdlib.h>
#include <time.h>

void initialize() {
	env_init(2048);
	srand(time(NULL));
}

value_t execute(const char *string) {
	return vm_run(block_parse(&string));
}
