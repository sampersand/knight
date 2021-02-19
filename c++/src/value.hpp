#pragma once

#include <variant>
#include <string>

namespace kn {
	using number = int;
	using string = std::string;

	class Literal;

	class Value {
	public:
		virtual ~Value() = default;

		virtual bool to_boolean() const = 0;
		virtual number to_number() const = 0;
		virtual string to_string() const = 0;

		virtual Literal run() const = 0;

		virtual void assign(std::shared_ptr<Value> value) const;
	};
}
