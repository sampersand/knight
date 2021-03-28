#ifndef KN_PARSE_H
#define KN_PARSE_H

#include "value.h" /* kn_value */

/*
 * Parses out a value from the given stream, updating the stream to reflect the
 * new value.
 *
 * If no value can be parsed, `KN_UNDEFINED` will be returned.
 */
kn_value kn_parse(const char **stream);

#endif /* !KN_PARSE_H */
