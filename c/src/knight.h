#ifndef KN_H
#define KN_H

#include "value.h" /* kn_value_t */

/*
 * Initializes Knight.
 *
 * This function should be called before any other Knight-related functions are
 * executed. It may be called multiple times, but only the first time actually
 * initializes knight; subsequent calls are ignored.
 *
 * This returns `1` if this is the first time being initialized, and `0` if it
 * has been initialized before.
 */
int kn_init(void);

/*
 * Frees all resources associated with knight Knight.
 *
 * Technically, this function is not needed. But it's a way to prove to valgrind
 * that i don't actually have unreachable memory.
 */
void kn_free(void);

/*
 * Parses and runs the given string in the global namespace.
 *
 * If any errors occur during the parsing or execution of the string, the
 * program is aborted.
 *
 * As all variables are global, any changes made to variables will be visible
 * to every other call to `kn_run`.
 */
struct kn_value_t kn_run(const char *stream);

#endif /* KN_H */
