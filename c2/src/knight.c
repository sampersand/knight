#include "knight.h"   /* prototypes, kn_value_t, kn_value_run, kn_value_free
                         KN_UNDEFINED, KN_ENV_DEFAULT_CAPACITY, size_t */
#include "function.h" /* kn_function_startup */
#include "parse.h"    /* kn_parse */
#include "shared.h"   /* die */
#include "string.h"   /* kn_string_startup, kn_string_shutdown */
#include "env.h"      /* kn_env_startup, kn_env_shutdown */

void kn_startup(size_t capacity) {
	static int is_function_initialized = 0;

	kn_env_startup(capacity ? capacity : KN_ENV_DEFAULT_CAPACITY);
	kn_string_startup();

	// we only want to initialize functions once, regardless of how many
	// times `kn_init` is called.
	if (!is_function_initialized) {
		is_function_initialized = 1;

		kn_function_startup();
	}
}

void kn_shutdown() {
	kn_env_shutdown();
	kn_string_shutdown();
}

kn_value_t kn_run(const char *stream) {
	kn_value_t parsed = kn_parse(&stream);

#ifndef KN_RECKLESS
	if (parsed == KN_UNDEFINED)
		die("unable to parse stream");
#endif /* KN_RECKLESS */

	kn_value_t ret = kn_value_run(parsed);
	kn_value_free(parsed);

	return ret;
}
