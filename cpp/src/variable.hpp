#pragma once

#include "knight.hpp"
#include "value.hpp"
#include "literal.hpp"

namespace kn {
	// A variable within Knight.
	//
	// As per the Knight specs, all variables are global.
	class Variable : public Value {
		// The name of the variable.
		std::string const name;

		// The value associated with this variable.
		//
		// Note that this is mutable because _logically_, assigning to a variable shouldn't change the _variable_ itself,
		// but we store the value within this class as an optimization.
		mutable SharedValue value;

		// Whether or not this value's been assigned to.
		mutable bool assigned;

	public:

		// Creates a new Variable with the given name.
		explicit Variable(std::string name) noexcept;

		// There is no default variable.
		Variable() = delete;

		// Parses an Variable out, or returns `nullptr` if the first character isn't a letter or `_`.
		static SharedValue parse(std::string_view& view);

		// Runs the variable, looking up its last assigned value.
		//
		// Throws an `Error` if the variable was never assigned.
		SharedValue run() const override;

		// Provides debugging output of this type.
		std::string dump() const override;

		// Assigns a value to this variable, discarding its previous value.
		void assign(SharedValue newvalue) const noexcept;
	};
}
