#pragma once 

namespace kn {
	inline void bug [[noreturn]] (char const string[]) {
		(void) string;
		throw "nope";
		/* todo */
	}
}
