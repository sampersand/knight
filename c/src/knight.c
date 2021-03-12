#include <time.h>   /* time */
#include <stdlib.h> /* srand */

#include "ast.h"    /* kn_ast_t, kn_ast_parse, kn_ast_run, kn_ast_free */
#include "knight.h" /* prototypes, kn_value_t */
#include "env.h"    /* kn_env_init */

// We define the environment size as starting at 256 identifiers.
#ifndef KNIGHT_ENV_INIT_SIZE
#define KNIGHT_ENV_INIT_SIZE 256
#endif

int kn_init() {
	// Ensure we only initialize knight once.
	static int INITIALIZED = 0;

	if (INITIALIZED) {
		return 0;
	}

	INITIALIZED = 1;
	kn_env_init(KNIGHT_ENV_INIT_SIZE);

	// seed the random number generator.
	srand(time(NULL));

	return 1;
}

void kn_free() {
	kn_env_free();
}

struct kn_value_t kn_run(const char *stream) {
	struct kn_ast_t ast = kn_ast_parse(&stream);
	struct kn_value_t ret = kn_ast_run(&ast);

	kn_ast_free(&ast);

	return ret;
}
