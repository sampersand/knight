#pragma once

#include "value.hpp"

namespace kn {
	// Initializes the Knight interpreter. This must be run before all other types are.
	void initialize();

	// Runs the input as Knight source code, returning its result.
	template<typename T>
	SharedValue run(T input) {
		std::string_view view(input);

		return Value::parse(view)->run();
	}
}
