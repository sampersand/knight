#pragma once

#include "value.hpp"
#include <variant>
#include <memory>

namespace kn {
	// The null type is simply monostate.
	using null = std::monostate;

	// The class that represents all forms of literal values within Knight.
	class Literal : public Value {
		// The data that's associated with this class.
		std::variant<null, bool, number, string> const data;

		// Checks to see if this value is a string.
		constexpr bool is_string() const noexcept;

		// If `this` is a string, returns true if `this` lexicographically compared to `rhs`. Otherwise,
		// returns the numerical comparison.
		int cmp(Value const& rhs) const;

	public:

		// Creates a new null value.
		explicit Literal() noexcept;

		// Creates a new boolean value.
		explicit Literal(bool boolean) noexcept;

		// Creates a new numeric value.
		explicit Literal(number num) noexcept;

		// Creates a new string value.
		explicit Literal(string str) noexcept;

		// Converts this class to a boolean.
		bool to_boolean() const override;

		// Converts this class to a number.
		number to_number() const override;

		// Converts this class to a string.
		string to_string() const override;

		// Attempts to parse a new Literal.
		static SharedValue parse(std::string_view& view);

		// Simply returns itself.
		SharedValue run() const override;

		// Provides debugging information about the literal.
		std::string dump() const override;

		// If `this` is a string, concatenates `this` with `rhs`. Otherwise, does mathematical addition.
		SharedValue operator+(Value const& rhs) const override;

		// Converts `rhs` to a number, and subtracts it from `this`.
		SharedValue operator-(Value const& rhs) const override;

		// If `this` is a string, duplicates it `rhs` times. Otherwise, does mathematical division.
		SharedValue operator*(Value const& rhs) const override;

		// Converts `rhs` to a number, and then divides `this` by it.
		//
		// Throws an `Error` if `rhs` is zero.
		SharedValue operator/(Value const& rhs) const override;

		// Converts `rhs` to a number, and then modulos `this` by it.
		//
		// Throws an `Error` if `rhs` is zero.
		SharedValue operator%(Value const& rhs) const override;

		// Raises `this` to the power of `rhs`.
		SharedValue pow(Value const& rhs) const override;

		// Compares `this` to `rhs`.
		//
		// This is the only Literal function that does not automatically convert `rhs` to a value.
		bool operator==(Value const& rhs) const override;

		// If `this` is a string, returns true if `this` is lexicographically less than `rhs`. Otherwise,
		// returns true if `this` is numerically less than `rhs`.
		bool operator<(Value const& rhs) const override;

		// If `this` is a string, returns true if `this` is lexicographically greater than `rhs`. Otherwise,
		// returns true if `this` is numerically greater than `rhs`.
		bool operator>(Value const& rhs) const override;
	};
}
