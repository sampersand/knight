#include "knight.h"   /* prototypes, kn_value, kn_value_run, kn_value_free
                         KN_UNDEFINED, size_t */
#include "function.h" /* kn_function_startup */
#include "parse.h"    /* kn_parse */
#include "shared.h"   /* die */
#include "string.h"   /* kn_string_startup, kn_string_shutdown */
#include "env.h"      /* kn_env_startup, kn_env_shutdown */

void kn_startup() {
	kn_env_startup();
	kn_string_startup();
	kn_function_startup();
}

void kn_shutdown() {
	kn_env_shutdown();
	kn_string_shutdown();
}

kn_value kn_run(const char *stream) {
	kn_value parsed = kn_parse(&stream);

#ifndef KN_RECKLESS
	if (parsed == KN_UNDEFINED)
		die("unable to parse stream");
#endif /* !KN_RECKLESS */

	kn_value ret = kn_value_run(parsed);
	kn_value_free(parsed);
	return ret;
}
