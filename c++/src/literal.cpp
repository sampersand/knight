#include <iostream>
#include "literal.hpp"
#include <memory>
#include <inttypes.h>

using namespace kn;

Literal::Literal() noexcept : data(null {}) {}
Literal::Literal(bool boolean) noexcept : data(boolean) {}
Literal::Literal(number num) noexcept : data(num) {}
Literal::Literal(string str) noexcept : data(str) {}


template <typename... Fns>
struct overload : Fns... {
  using Fns::operator()...;
};

template <typename... Fns>
overload(Fns...) -> overload<Fns...>;

bool Literal::to_boolean() const {
	return std::visit(overload {
		[](null const&) { return false; },
		[](bool const& boolean) { return boolean; },
		[](number const& num) { return num != 0; },
		[](string const& str) { return str.length() != 0; }
	}, data);
}

number Literal::to_number() const {
	return std::visit(overload {
		[](null const&) { return (number) 0; },
		[](bool const& boolean) { return (number) boolean; },
		[](number const& num) { return num; },
		[](string const& str) { return std::stoll(str); }
	}, data);
}

string Literal::to_string() const {
	return std::visit(overload {
		[](null const&) { return string("null"); },
		[](bool const& boolean) { return string(boolean ? "true" : "false"); },
		[](number const& num) { return std::to_string(num); },
		[](string const& str) { return str; }
	}, data);
}

#define MAKE_SHARED(x) (std::make_shared<Literal>(x))

SharedValue Literal::parse(std::string_view& view) {
	char front = view.front();
	SharedValue literal;

	switch (front) {
		case 'N':
			literal = MAKE_SHARED();
			goto remove_keyword;

		case 'T':
		case 'F':
			literal = MAKE_SHARED(front == 'T');
		remove_keyword:
			do {
 				view.remove_prefix(1);
			} while (std::isupper(view.front()));

			return literal;

		case '\'':
		case '\"': {
			view.remove_prefix(1);
			auto begin = view.cbegin();

			for(char quote = front; quote != view.front(); view.remove_prefix(1)) {
				if (view.empty()) {
					throw std::invalid_argument("unmatched quote encountered!");
				}
			}

			string ret(begin, view.cbegin());
			view.remove_prefix(1);

			return MAKE_SHARED(ret);
		}

		default:
			if (!std::isdigit(front)) {
				return nullptr;
			}

			number num = 0;

			for (; std::isdigit(front = view.front()); view.remove_prefix(1)) {
				num = num * 10 + (front - '0');
			}

			return MAKE_SHARED(num);
		}
}

SharedValue Literal::run() const {
	return shared_from_this();
}

inline bool Literal::is_string() const noexcept {
	return std::holds_alternative<string>(data);
}

SharedValue Literal::operator+(Value const& rhs) const {
	if (!is_string()) {
		return MAKE_SHARED(to_number() + rhs.to_number());
	}

	string ret(std::get<string>(data));
	ret += rhs.to_string();

	return MAKE_SHARED(ret);
}

SharedValue Literal::operator-(Value const& rhs) const {
	return MAKE_SHARED(to_number() - rhs.to_number());
}

SharedValue Literal::operator*(Value const& rhs) const {
	if (!is_string()) {
		return MAKE_SHARED(to_number() * rhs.to_number());
	}

	number rhs_num = rhs.to_number();

	if (rhs_num < 0) {
		throw std::invalid_argument("cannot duplicate by a negative number");
	}

	string const& str = std::get<string>(data);
	string ret;

	for (auto i = 0; i < rhs_num; ++i) {
		ret += str;
	}

	return MAKE_SHARED(ret);
}

SharedValue Literal::operator/(Value const& rhs) const {
	auto rhs_num = rhs.to_number();

	if (rhs_num == 0) {
		throw std::domain_error("cannot divide by zero!\n");
	}

	return MAKE_SHARED(to_number() / rhs_num);
}

SharedValue Literal::operator%(Value const& rhs) const {
	auto rhs_num = rhs.to_number();

	if (rhs_num == 0) {
		throw std::domain_error("cannot modulo by zero!\n");
	}

	return MAKE_SHARED(to_number() % rhs_num);
}

SharedValue Literal::pow(Value const& rhs) const {
	number ret = 1;
	number base = to_number();
	number exp = rhs.to_number();

	for (; exp; --exp) {
		ret *= base;
	}

	return MAKE_SHARED(ret);
}

bool Literal::operator==(Value const& rhs_value) const {
	auto rhs = dynamic_cast<Literal const*>(&rhs_value);

	if (!rhs || data.index() != rhs->data.index()) {
		return false;
	}

	return std::visit(overload {
		[](null const&) { return true; },
		[&](bool const& boolean) { return boolean == std::get<bool>(rhs->data); },
		[&](number const& num) { return num == std::get<number>(rhs->data); },
		[&](string const& str) { return str == std::get<string>(rhs->data); }
	}, data);
}

int Literal::cmp(Value const& rhs) const {
	if (!is_string()) {
		auto this_num = to_number();
		auto rhs_num = rhs.to_number();

		return this_num < rhs_num ? -1 : this_num > rhs_num ? 1 : 0;
	}

	string const & this_string = std::get<string>(data);
	string rhs_string = rhs.to_string();


	auto pair = std::mismatch(
		this_string.cbegin(), this_string.cend(),
		rhs_string.cbegin(), rhs_string.cend()
	);

	return pair.first < pair.second ? -1 : pair.first > pair.second ? 1 : 0;
}

bool Literal::operator<(Value const& rhs) const {
	return cmp(rhs) < 0;
}

bool Literal::operator>(Value const& rhs) const {
	return cmp(rhs) > 0;
}
