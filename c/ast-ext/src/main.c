#include "knight.h" /* kn_startup, kn_run, kn_value_free, kn_shutdown */
#include "shared.h" /* die, xmalloc, xrealloc */

#include <string.h> /* strlen */
#include <stdlib.h> /* free */
#include <stdio.h>  /* FILE, fopen, sterror, feof, fread, fclose */
#include <errno.h>  /* errno */

static char *read_file(const char *filename) {
	FILE *file = fopen(filename, "r");

#ifndef KN_RECKLESS
	if (file == NULL)
		die("unable to read file '%s': %s", filename, strerror(errno));
#endif /* !KN_RECKLESS */

	size_t length = 0;
	size_t capacity = 2048;
	char *contents = xmalloc(capacity);

	while (!feof(file)) {
		size_t amntread = fread(&contents[length], 1, capacity - length, file);

		if (amntread == 0) {

#ifndef KN_RECKLESS
		if (!feof(stdin))
			die("unable to line in file '%s': %s'", filename, strerror(errno));
#endif /* !KN_RECKLESS */

			break;
		}

		length += amntread;

		if (length == capacity) {
			capacity *= 2;
			contents = xrealloc(contents, capacity);
		}
	}

	if (fclose(file) == EOF) {

#ifndef KN_RECKLESS
		perror("couldn't close input file");
#endif /* !KN_RECKLESS */

	}

	return xrealloc(contents, length);
}

static void usage(char *program) {
	die("usage: %s (-e 'expr' | -f file)", program);
}

int main(int argc, char **argv) {
	char *str;

	if (argc != 3 || strlen(argv[1]) != 2 || argv[1][0] != '-')
		usage(argv[0]);

	switch (argv[1][1]) {
	case 'e':
		str = argv[2];
		break;
	case 'f':
		str = read_file(argv[2]);
		break;
	default:
		usage(argv[0]);
	}

	kn_startup();

#ifdef KN_RECKLESS
	kn_run(str);
#else
	kn_value_free(kn_run(str));
	kn_shutdown();

	if (argv[1][1] == 'f')
		free(str);
#endif /* KN_RECKLESS */

	return 0;
}
