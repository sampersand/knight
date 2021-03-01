#define HELPFUL
#include "value.h"
#include "parse.h"
#include <string.h>
#include <stdio.h>

int main(int argc, const char **argv) {
	(void) argc;
	const char **stream = &argv[1];
	kn_value_t value = kn_parse(stream);

	kn_value_dump(value);
	kn_value_run(value);
	kn_value_free(value);
	printf("\n");
	printf("\n");
	// kn_value_t value = kn_value_new_number(123);
	// kn_value_dump(value);
	// kn_value_free(kn_value_clone(value));
	// kn_value_free(value);

	// value = kn_value_new_boolean(true);
	// kn_value_dump(value);
	// kn_value_free(kn_value_clone(value));
	// kn_value_free(value);

	// value = kn_value_new_boolean(false);
	// kn_value_dump(value);
	// kn_value_free(kn_value_clone(value));
	// kn_value_free(value);

	// value = kn_value_new_identifier(kn_string_new(strdup("foo")));
	// kn_value_dump(value);
	// kn_value_free(kn_value_clone(value));
	// kn_value_free(value);

	// value = kn_value_new_string(kn_string_new(strdup("foo")));
	// kn_value_dump(value);
	// kn_value_free(kn_value_clone(value));
	// kn_value_free(value);
}
