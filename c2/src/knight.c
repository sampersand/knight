#include "knight.h"
#include "function.h"
#include "parse.h"
#include "shared.h"
#include "env.h"

#ifndef DEFAULT_CAPACITY
#define DEFAULT_CAPACITY 256
#endif

void kn_init(size_t capacity) {
	kn_env_init(capacity ? capacity : DEFAULT_CAPACITY);
	kn_string_init();
	kn_function_init();
}

void kn_free() {
	kn_env_free();
}

kn_value_t kn_run(const char *stream) {
	kn_value_t parsed = kn_parse(&stream);

	if (parsed == KN_UNDEFINED)
		die("unable to parse stream");

	kn_value_t ret = kn_value_run(parsed);
	kn_value_free(parsed);

	return ret;
}
