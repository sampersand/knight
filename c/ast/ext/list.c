#include "../src/custom.h"
#include "../src/function.h"

struct list {
	unsigned length, capacity;
	kn_value *data;
};

struct list empty()