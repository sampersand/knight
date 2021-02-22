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
struct overload : Fns... { using Fns::operator()...; };

template <typename... Fns>
overload(Fns...) -> overload<Fns...>;

bool Literal::to_boolean() const {
	return std::visit(overload {
		[](null) { return false; },
		[](bool boolean) { return boolean; },
		[](number num) { return num != 0; },
		[](string const& str) { return str.length() != 0; }
	}, data);
}

number Literal::to_number() const {
	return std::visit(overload {
		[](null) { return (number) 0; },
		[](bool boolean) { return (number) boolean; },
		[](number num) { return num; },
		[](string const& str) { return std::stoll(str); }
	}, data);
}

string Literal::to_string() const {
	return std::visit(overload {
		[](null) { return string("null"); },
		[](bool boolean) { return string(boolean ? "true" : "false"); },
		[](number num) { return std::to_string(num); },
		[](string const& str) { return str; }
	}, data);
}

SharedValue Literal::parse(std::string_view& view) {
	char front = view.front();
	SharedValue literal;

	switch (front) {
		case 'N':
			literal = std::make_shared<Literal>();
			goto remove_keyword;

		case 'T':
		case 'F':
			literal = std::make_shared<Literal>(front == 'T');
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

			return std::make_shared<Literal>(ret);
		}

		default:
			if (!std::isdigit(front)) {
				return nullptr;
			}

			number num = 0;

			for (; std::isdigit(front = view.front()); view.remove_prefix(1)) {
				num = num * 10 + (front - '0');
			}

			return std::make_shared<Literal>(num);
		}
}

SharedValue Literal::run() const {
	return shared_from_this();
}

constexpr bool Literal::is_string() const noexcept {
	return std::holds_alternative<string>(data);
}

SharedValue Literal::operator+(Value const& rhs) const {
	if (!is_string()) {
		return std::make_shared<Literal>(to_number() + rhs.to_number());
	}

	string ret(std::get<string>(data));
	ret += rhs.to_string();

	return std::make_shared<Literal>(ret);
}

SharedValue Literal::operator-(Value const& rhs) const {
	return std::make_shared<Literal>(to_number() - rhs.to_number());
}

SharedValue Literal::operator*(Value const& rhs) const {
	if (!is_string()) {
		return std::make_shared<Literal>(to_number() * rhs.to_number());
	}

	number rhs_num = rhs.to_number();

	if (rhs_num < 0) {
		throw Error("cannot duplicate by a negative number");
	}

	string const& str = std::get<string>(data);
	string ret;

	for (auto i = 0; i < rhs_num; ++i) {
		ret += str;
	}

	return std::make_shared<Literal>(ret);
}

SharedValue Literal::operator/(Value const& rhs) const {
	auto rhs_num = rhs.to_number();

	if (rhs_num == 0) {
		throw Error("cannot divide by zero!\n");
	}

	return std::make_shared<Literal>(to_number() / rhs_num);
}

SharedValue Literal::operator%(Value const& rhs) const {
	auto rhs_num = rhs.to_number();

	if (rhs_num == 0) {
		throw Error("cannot modulo by zero!\n");
	}

	return std::make_shared<Literal>(to_number() % rhs_num);
}

SharedValue Literal::pow(Value const& rhs) const {
	number ret = 1;
	number base = to_number();
	number exp = rhs.to_number();

	for (; exp; --exp) {
		ret *= base;
	}

	return std::make_shared<Literal>(ret);
}

bool Literal::operator==(Value const& rhs_value) const {
	auto rhs = dynamic_cast<Literal const*>(&rhs_value);

	if (!rhs || data.index() != rhs->data.index()) {
		return false;
	}

	return std::visit(overload {
		[](null) { return true; },
		[=](bool boolean) { return boolean == std::get<bool>(rhs->data); },
		[=](number num) { return num == std::get<number>(rhs->data); },
		[=](string const& str) { return str == std::get<string>(rhs->data); }
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


	return this_string < rhs_string ? -1 : this_string > rhs_string ? 1 : 0;
	// auto pair = std::mismatch(
	// 	this_string.cbegin(), this_string.cend(),
	// 	rhs_string.cbegin(), rhs_string.cend()
	// );

	// return pair.first < pair.second ? -1 : pair.first > pair.second ? 1 : 0;
}

bool Literal::operator<(Value const& rhs) const {
	return cmp(rhs) < 0;
}

bool Literal::operator>(Value const& rhs) const {
	return cmp(rhs) > 0;
}
