#pragma once

#include "error.hpp"
#include <string>
#include <string_view>

namespace kn {
	// The type used by numbers within Knight.
	using number = long long;

	// The type used by strings within Knight.
	using string = std::string;

	// Note that Knight's booleans are represented via `bool`.

	// Declare the `SharedValue` alias because we use it so often.
	class Value;
	using SharedValue = std::shared_ptr<Value const>;

	// The class that all entities within Knight must subclass from.
	class Value : public std::enable_shared_from_this<Value> {
	public:

		// Required so we don't leak memory when dropping types.
		virtual ~Value() = default;

		// All implementing types must provide a `run` function.
		virtual SharedValue run() const = 0;

		// The dump function is used for debugging.
		virtual std::string dump() const = 0;

		// Parses a value from the given view. Implementing types should provide their own logic.
		static SharedValue parse(std::string_view& view);

		// Assigns a `value` to `this`.
		//
		// The default implementation converts `this` to a string, and then and runs `Identifier`'s `assign` method.
		virtual SharedValue assign(SharedValue value) const;

		// Converts this class to a boolean.
		//
		// The default implementation calls `to_boolean` on the result of `run`ning `this`.
		virtual bool to_boolean() const;

		// Converts this class to a number.
		//
		// The default implementation calls `number` on the result of `run`ning `this`.
		virtual number to_number() const;

		// Converts this class to a string.
		//
		// The default implementation calls `to_string` on the result of `run`ning `this`.
		virtual string to_string() const;

		// Adds `rhs` to `this`.
		//
		// The default implementation simply `run`s both sides and calls `operator+` again.
		virtual SharedValue operator+(Value const& rhs) const;

		// Subtracts `rhs` from `this`.
		//
		// The default implementation simply `run`s both sides and calls `operator-` again.
		virtual SharedValue operator-(Value const& rhs) const;

		// Multiplies `this` by `rhs`.
		//
		// The default implementation simply `run`s both sides and calls `operator*` again.
		virtual SharedValue operator*(Value const& rhs) const;

		// Divides `this` by `rhs`.
		//
		// The default implementation simply `run`s both sides and calls `operator/` again.
		virtual SharedValue operator/(Value const& rhs) const;

		// Modulos `this` by `rhs`.
		//
		// The default implementation simply `run`s both sides and calls `operator%` again.
		virtual SharedValue operator%(Value const& rhs) const;

		// Raises `this` to the power of `rhs`.
		//
		// The default implementation simply `run`s both sides and calls `pow` again.
		virtual SharedValue pow(Value const& rhs) const;

		// Checks to see if `this` to equal to `rhs`.
		//
		// The default implementation simply `run`s both sides and calls `operator==` again.
		virtual bool operator==(Value const& rhs) const;

		// Checks to see if `this` is less than `rhs`.
		//
		// The default implementation simply `run`s both sides and calls `operator<` again.
		virtual bool operator<(Value const& rhs) const;

		// Checks to see if `this` is greater than `rhs`.
		//
		// The default implementation simply `run`s both sides and calls `operator>` again.
		virtual bool operator>(Value const& rhs) const;
	};
}
