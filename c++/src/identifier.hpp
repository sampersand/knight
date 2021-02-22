#pragma once

#include "knight.hpp"
#include "value.hpp"
#include "literal.hpp"

namespace kn {
	// An identifier within Knight.
	//
	// As per the Knigth specs, all identifiers are global.
	class Identifier : public Value {
		// The name of the identifier.
		std::string const name;

	public:

		// Creates a new Identifier with the given name.
		explicit Identifier(std::string name) noexcept;

		// There is no default Identifier.
		Identifier() = delete;

		// Parses an identifier out, or returns `nullptr` if the first character isn't a letter or `_`.
		static SharedValue parse(std::string_view& view);

		// Runs the identifier, looking up its last assigned value.
		//
		// Throws `UnknownIdentifier` if the identifier was never assigned.
		SharedValue run() const override;

		// Assigns a value to this Identifier, discarding its previous value.
		//
		// This will run `value`, and returns the result of running it.
		SharedValue assign(SharedValue value) const override;
	};
}
