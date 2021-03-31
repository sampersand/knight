#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "knight.h"
#include "shared.h"

void usage(const char *program_name) {
	die("usage: %s [-e program] [-f file]", program_name);
}

char *read_file(const char *filename) {
	FILE *file = fopen(filename, "r");

	if (file == NULL) {
		die("unable to read file '%s': %s", filename, strerror(errno));
	}

	size_t len = 0;
	size_t cap = 2048;
	char *contents = xmalloc(cap);

	while (!feof(file)) {
		size_t nchars = fread(&contents[len], 1, cap - len, file);

		if (nchars == 0) {
			if (feof(stdin)) {
				break;
			} else {
				die("unable to line in file '%s': %s'",
					filename, strerror(errno));
			}
		}

		len += nchars;

		if (len == cap) {
			cap *= 2;
			contents = xrealloc(contents, cap);
		}
	}

	fclose(file);
	return xrealloc(contents, len);
}

int main(int argc, const char **argv) {
	// note: to keep it cross-platform, i opted not to use optparse.
	if (argc != 3) {
		usage(argv[0]);
	}

 	if (strlen(argv[1]) != 2 || argv[1][0] != '-') {
 		usage(argv[0]);
 	}

	const char *string;

	switch(argv[1][1]) {
	case 'e':
		string = argv[2];
		break;
	case 'f':
		string = read_file(argv[2]);
		break;
	default:
		usage(argv[0]);
	}

	kn_init();

	struct kn_value_t ret = kn_run(string);

	kn_value_free(&ret);

	// kn_free();

	return 0;
}
