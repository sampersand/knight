#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "knight.h"
#include "value.h"
#include "shared.h"

static char *read_file(const char *filename) {
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
			if (feof(stdin))
				break;

			die("unable to line in file '%s': %s'", filename, strerror(errno));
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

int main(int argc, const char *argv[]) {
	// note: to keep it cross-platform, i opted not to use optparse.
	if (argc != 3)
		goto usage;

	if (strlen(argv[1]) != 2 || argv[1][0] != '-')
		goto usage;

	const char *string;

	switch(argv[1][1]) {
	case 'e':
		string = argv[2];
		break;
	case 'f':
		string = read_file(argv[2]);
		break;
	default:
		goto usage;
	}

	kn_startup();
	kn_value_free(kn_run(string));
	kn_shutdown();

	return 0;

usage:

	die("usage: %s [-e program] [-f file]", argv[0]);
}
