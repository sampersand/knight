#pragma once

#include "value.hpp"
#include <memory>

namespace kn {
	using null = std::monostate;

	class Literal : public Value {
		std::variant<null, bool, number, string> const data;

		bool is_string() const noexcept;
		int cmp(Literal const& rhs) const;
	public:
		Literal() noexcept;
		Literal(bool boolean) noexcept;
		Literal(number num) noexcept;
		Literal(string str) noexcept;

		bool to_boolean() const override;
		number to_number() const override;
		string to_string() const override;

		static std::shared_ptr<Value const> parse(std::string_view& view) override;
		std::shared_ptr<Value const> run() const override;

		Literal operator+(Literal const& rhs) const;
		Literal operator-(Literal const& rhs) const;
		Literal operator*(Literal const& rhs) const;
		Literal operator/(Literal const& rhs) const;
		Literal operator%(Literal const& rhs) const;
		Literal pow(Literal const& rhs) const;
		bool operator==(Literal const& rhs) const;
		bool operator<(Literal const& rhs) const;
		bool operator>(Literal const& rhs) const;
	};
}
