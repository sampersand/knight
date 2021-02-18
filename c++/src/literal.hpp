#pragma once

#include "value.hpp"

namespace kn {
	using null = std::monostate;
	using string = std::string;

	class Literal : Value {
		std::variant<null, bool, number, std::string> data;

		bool is_string() const;
		int cmp(const Literal& rhs) const;
	public:
		Literal();
		Literal(bool boolean);
		Literal(number num);
		Literal(std::string string);

		bool to_boolean() const;
		number to_number() const;
		std::string to_string() const;

		Literal operator+(const Literal& rhs) const;
		Literal operator-(const Literal& rhs) const;
		Literal operator*(const Literal& rhs) const;
		Literal operator/(const Literal& rhs) const;
		Literal operator%(const Literal& rhs) const;
		Literal pow(const Literal& rhs) const;
		bool operator==(const Literal& rhs) const;
		bool operator<(const Literal& rhs) const;
		bool operator>(const Literal& rhs) const;
	};
}
