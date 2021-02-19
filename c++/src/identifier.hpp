#pragma once

#include "value.hpp"

namespace kn {
	struct UnknownIdentifier : std::runtime_error {
		std::string const ident;

		UnknownIdentifier(std::string ident);
	};

	class Identifier : public Value {
		std::string name;
	public:
		Identifier(std::string name) noexcept;

		static std::shared_ptr<Value const> parse(std::string_view& view) override;
		std::shared_ptr<Value const> run() const override;
		void assign(std::shared_ptr<Value const> value) const override;
	};
}
