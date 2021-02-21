#pragma once

#include <variant>
#include <string>
#include <string_view>

namespace kn {
	using number = long long;
	using string = std::string;

	class Literal;
	class Value;
	using SharedValue = std::shared_ptr<Value const>;

	class Value : public std::enable_shared_from_this<Value> {
	public:
		virtual ~Value() = default;
		virtual SharedValue run() const = 0;

		static SharedValue parse(std::string_view& view);

		template<typename S>
		static SharedValue parse_str(S input) {
			std::string_view view{input};
			return Value::parse(view);
		}

		virtual SharedValue assign(SharedValue value) const;

		virtual bool to_boolean() const;
		virtual number to_number() const;
		virtual string to_string() const;

		virtual SharedValue operator+(Value const& rhs) const;
		virtual SharedValue operator-(Value const& rhs) const;
		virtual SharedValue operator*(Value const& rhs) const;
		virtual SharedValue operator/(Value const& rhs) const;
		virtual SharedValue operator%(Value const& rhs) const;
		virtual SharedValue pow(Value const& rhs) const;
		virtual bool operator==(Value const& rhs) const;
		virtual bool operator<(Value const& rhs) const;
		virtual bool operator>(Value const& rhs) const;

	};
}
