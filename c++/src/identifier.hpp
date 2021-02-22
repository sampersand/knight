#pragma once

#include "value.hpp"

namespace kn {
	struct UnknownIdentifier : std::exception {
		std::string ident;
		std::string message;

		UnknownIdentifier(std::string ident);

  		const char* what() const noexcept;
  	};

	class Identifier : Value {
		std::string name;
	public:
		Identifier(std::string name);

		bool to_boolean() const override;
		number to_number() const override;
		string to_string() const override;

		Literal run() const override;
		void assign(std::shared_ptr<Value> value) const override;
	};
}
