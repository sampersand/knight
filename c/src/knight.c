#include "knight.h"
#include "env.h"
#include <time.h>
#include <stdlib.h>


#ifndef KNIGHT_ENV_INIT_SIZE
#define KNIGHT_ENV_INIT_SIZE 128
#endif

void kn_init(void) {
	static bool INITIALIZED = false;

	if (INITIALIZED) {
		return;
	}

	INITIALIZED = true;

	kn_env_init(KNIGHT_ENV_INIT_SIZE);
	srand(time(NULL));
}

struct kn_value_t kn_run(const char *stream) {
	struct kn_ast_t ast = kn_ast_parse(&stream);
	struct kn_value_t ret = kn_ast_run(&ast);

	kn_ast_free(&ast);

	return ret;
}
