#ifndef VALUE_H
#define VALUE_H

#include <inttypes.h>
#include <stdbool.h>
#include "function.h"
#include "shared.h"

typedef intmax_t number_t;
typedef char * string_t;

enum value_kind_t {
	VK_INTEGER,
	VK_BOOLEAN,
	VK_NULL,
	VK_STRING,
	VK_IDENTIFIER,
	VK_FUNCTION,
};

struct value_t {
	enum value_kind_t kind;

	union {
		number_t integer;
		bool boolean;
		struct {
			union {
				string_t string;
				char *identifier;
				struct {
					struct function_t *function;
					struct value_t *args;
				};
			};
			unsigned *rc;
		};
	};
};

inline struct value_t value_new_intern(string_t string) {
	return (struct value_t) {
		.kind = VK_STRING,
		.string = string,
		.rc = NULL
	};
}

inline struct value_t value_new_string(string_t string) {
	unsigned *rc = xmalloc(sizeof(unsigned));
	++*rc;

	return (struct value_t) {
		.kind = VK_STRING,
		.string = string,
		.rc = rc
	};
}

struct value_t EMPTY_STRING = (struct value_t) {
	.kind = VK_STRING,
	.string = "",
	.rc = NULL
};

void value_free(struct value_t *);
struct value_t value_clone(const struct value_t *);
struct value_t value_run(const struct value_t *);

number_t value_to_integer(const struct value_t *);
bool value_to_boolean(const struct value_t *);

#define INTERN_BIT 1

// _not_ a string, as it might be an intern.
size_t value_to_string(const struct value_t *);

#endif
