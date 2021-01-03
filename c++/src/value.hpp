#pragma once

#include <cinttypes>
#include <string>
#include <variant>


namespace kn {
	using integer_t = intmax_t;

	class value {
		std::variant<bool, integer_t, std::string> data;
	public:
		value(bool boolean);
		value(integer_t integer);
		value(std::string string);

		bool to_boolean() const;
		integer_t to_integer() const;
		std::string to_string() const;

		value operator+(const value& rhs) const;
		value operator-(const value& rhs) const;
		value operator*(const value& rhs) const;
		value operator/(const value& rhs) const;
		value operator%(const value& rhs) const;
		value pow(const value& rhs) const;
		bool operator!() const;
		bool operator<(const value& rhs) const;
		bool operator>(const value& rhs) const;
		bool operator==(const value& rhs) const;
	};
}
