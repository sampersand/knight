#include "knight.h"
#include "function.h"
#include "parse.h"
#include "env.h"

void kn_init() {
	kn_env_init(256);
	kn_function_initialize();
}

void kn_free() {
	kn_env_free();
}

kn_value_t kn_run(const char *stream) {
	kn_value_t value = kn_parse(&stream);
	kn_value_t ret = kn_value_run(value);
	kn_value_free(value);
	return ret;
}
