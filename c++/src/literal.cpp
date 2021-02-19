#include "literal.hpp"
#include <memory>

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
		[](null const&) { return 0; },
		[](bool const& boolean) { return (number) boolean; },
		[](number const& num) { return num; },
		[](string const& str) { return std::stoi(str); }
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

std::shared_ptr<Value const> Literal::parse(std::string_view& view) {
	(void) view;
	return nullptr;
}

std::shared_ptr<Value const> Literal::run() const {
	return shared_from_this();
}

inline bool Literal::is_string() const noexcept {
	return std::holds_alternative<string>(data);
}

Literal Literal::operator+(Literal const& rhs) const {
	if (!is_string()) {
		return Literal(to_number() + rhs.to_number());
	}

	string ret(std::get<string>(data));
	ret += rhs.to_string();

	return ret;
}

Literal Literal::operator-(Literal const& rhs) const {
	return to_number() - rhs.to_number();
}

Literal Literal::operator*(Literal const& rhs) const {
	if (!is_string()) {
		return to_number() * rhs.to_number();
	}

	number rhs_num = rhs.to_number();

	if (rhs_num < 0) {
		throw std::invalid_argument("cannot duplicate by a negative number");
	}

	string const& str = std::get<string>(data);
	string ret(str.length() * (size_t) rhs_num, '\0');

	for (auto i = 0; i < rhs_num; ++i) {
		ret += str;
	}

	return ret;
}

Literal Literal::operator/(Literal const& rhs) const {
	auto rhs_num = rhs.to_number();

	if (rhs_num == 0) {
		throw std::domain_error("cannot divide by zero!\n");
	}

	return to_number() / rhs_num;
}

Literal Literal::operator%(Literal const& rhs) const {
	auto rhs_num = rhs.to_number();

	if (rhs_num == 0) {
		throw std::domain_error("cannot modulo by zero!\n");
	}

	return to_number() % rhs_num;
}

Literal Literal::pow(Literal const& rhs) const {
	number ret = 1;
	number base = to_number();
	number exp = rhs.to_number();

	for (; exp; --exp) {
		ret *= base;
	}

	return ret;
}

bool Literal::operator==(Literal const& rhs) const {
	if (data.index() != rhs.data.index()) {
		return false;
	}

	return std::visit(overload {
		[](null const&) { return true; },
		[&](bool const& boolean) { return boolean == std::get<bool>(rhs.data); },
		[&](number const& num) { return num == std::get<number>(rhs.data); },
		[&](string const& str) { return str == std::get<string>(rhs.data); }
	}, data);
}

int Literal::cmp(Literal const& rhs) const {
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

bool Literal::operator<(Literal const& rhs) const {
	return cmp(rhs) < 0;
}

bool Literal::operator>(Literal const& rhs) const {
	return cmp(rhs) > 0;
}
