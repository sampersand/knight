#pragma once

#include "value.hpp"
#include "literal.hpp"

namespace kn {
	struct UnknownIdentifier : std::runtime_error {
		std::string const ident;

		UnknownIdentifier(std::string ident);
	};

	class Identifier : public Value {
		std::string const name;
	public:
		explicit Identifier(std::string name) noexcept;
		Identifier() = delete;

		static SharedValue parse(std::string_view& view);

		SharedValue run() const override;
		SharedValue assign(SharedValue value) const override;
	};
}
