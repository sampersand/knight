#pragma once

#include <variant>
#include <string>

namespace kn {
	using number = int;

	class Value {
	public:
		virtual bool to_boolean() const = 0;
		virtual number to_number() const = 0;
		virtual std::string to_string() const = 0;

		// value operator+(const value& rhs) const;
		// value operator-(const value& rhs) const;
		// value operator*(const value& rhs) const;
		// value operator/(const value& rhs) const;
		// value operator%(const value& rhs) const;
		// value pow(const value& rhs) const;
		// bool operator!() const;
		// bool operator<(const value& rhs) const;
		// bool operator>(const value& rhs) const;
		// bool operator==(const value& rhs) const;
	};
}
