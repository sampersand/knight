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

#ifdef KN_EXT_FUNCTION
/*
 * This function is called whenever a keyword function starting with `X` is
 * encountered.
 *
 * The passed `stream` will have only the leading `X` removed, and the function
 * should strip the and any other relevant trailing characters before returning.
 */
kn_value kn_parse_extension(const char **stream);
#endif /* KN_EXT_FUNCTION */
	
#endif /* !KN_PARSE_H */
