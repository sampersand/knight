#pragma once

#include "value.hpp"
#include <iostream>

namespace kn {
	// Initializes the Knight interpreter. This must be run before all other types are.
	void initialize();

	// Runs the input as Knight source code, returning its result.
	template<typename T>
	Value run(T input) {
		std::string_view view(input);
		auto value = Value::parse(view);

		if (!value)
			throw Error("cannot parse a value");

		return value->run();
	}
}
