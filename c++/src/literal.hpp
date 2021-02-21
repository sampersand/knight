#pragma once

#include "value.hpp"
#include <memory>

namespace kn {
	using null = std::monostate;

	class Literal : public Value {
		std::variant<null, bool, number, string> const data;

		bool is_string() const noexcept;
		int cmp(Value const& rhs) const;
	public:
		Literal() noexcept;
		Literal(bool boolean) noexcept;
		Literal(number num) noexcept;
		Literal(string str) noexcept;

		bool to_boolean() const override;
		number to_number() const override;
		string to_string() const override;

		static SharedValue parse(std::string_view& view);
		SharedValue run() const override;

		SharedValue operator+(Value const& rhs) const override;
		SharedValue operator-(Value const& rhs) const override;
		SharedValue operator*(Value const& rhs) const override;
		SharedValue operator/(Value const& rhs) const override;
		SharedValue operator%(Value const& rhs) const override;
		SharedValue pow(Value const& rhs) const override;
		bool operator==(Value const& rhs) const override;
		bool operator<(Value const& rhs) const override;
		bool operator>(Value const& rhs) const override;
	};
}
