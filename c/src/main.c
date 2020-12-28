#include <string.h>
#include <stdio.h>

#include "knight.h"
#include "shared.h"


void usage(const char *program_name) {
	die("usage: %s (-e program | -f file)", program_name);
}

int main(int argc, const char **argv) {
	if (argc == 1 || argc > 3 || strlen(argv[1]) != 2) {
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

	kn_init();

	struct kn_value_t ret = kn_run(string);

	kn_value_free(&ret);

	return 0;
}
