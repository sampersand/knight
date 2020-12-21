#include <string.h>
#include <stdio.h>

#include "shared.h"
#include "env.h"
#include "ast.h"
#include "value.h"


void usage(const char *program_name) {
	die("usage: %s (-e program | -f file)", program_name);
}

int main(int argc, const char **argv) {
	if (argc != 3 || strlen(argv[1]) != 2) {
		usage(argv[0]);
	}

	const char *string;

	switch(argv[1][1]) {
	case 'e':
		string = argv[2];
		break;
	case 'f':
		die("todo: read from a file");
	default:
		usage(argv[0]);
	}

	kn_env_initialize(128);

	struct kn_ast_t *ast = kn_ast_parse(&string);
	struct kn_value_t ret = kn_ast_run(ast);

	kn_value_free(&ret);
	kn_ast_free(ast);
}
