#pragma once

#include "value.hpp"

namespace kn {
	using null = std::monostate;

	struct ZeroDivisionError : public std::exception {
		const char* what() const noexcept;
	};

	class Literal : public Value {
		std::variant<null, bool, number, string> data;

		bool is_string() const;
		int cmp(const Literal& rhs) const;
	public:
		Literal();
		Literal(bool boolean);
		Literal(number num);
		Literal(string str);

		bool to_boolean() const override;
		number to_number() const override;
		string to_string() const override;

		std::shared_ptr<Value>& run() const override;

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
