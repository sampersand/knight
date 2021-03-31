#pragma once

#include <string>
#include <stdexcept>
#include <ostream>

namespace kn {
	// the base class for all errors within Knight.
	struct Error : public std::runtime_error {
		// Creates a new error with the given message.
		explicit Error(std::string const& what_arg);
	};
}
