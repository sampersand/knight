#include "literal.hpp"
#include "shared.hpp"

using namespace kn;

Literal::Literal() : data(null {}) {}
Literal::Literal(bool boolean) : data(boolean) {}
Literal::Literal(number num) : data(num) {}
Literal::Literal(string str) : data(str) {}

template <typename... Fns>
struct overload : Fns... {
  using Fns::operator()...;
};

template <typename... Fns>
overload(Fns...) -> overload<Fns...>;

bool Literal::to_boolean() const {
	return std::visit(overload {
		[](const null&) { return false; },
		[](const bool& boolean) { return boolean; },
		[](const number& num) { return num != 0; },
		[](const string& str) { return str.length() != 0; }
	}, data);
}

number Literal::to_number() const {
	return std::visit(overload {
		[](const null&) { return 0; },
		[](const bool& boolean) { return (number) boolean; },
		[](const number& num) { return num; },
		[](const string& str) { return std::stoi(str); }
	}, data);
}

string Literal::to_string() const {
	return std::visit(overload {
		[](const null&) { return string("null"); },
		[](const bool& boolean) { return string(boolean ? "true" : "false"); },
		[](const number& num) { return std::to_string(num); },
		[](const string& str) { return str; }
	}, data);
}


inline bool Literal::is_string() const {
	return std::holds_alternative<string>(data);
}

Literal Literal::operator+(const Literal& rhs) const {
	if (!is_string()) {
		return new Literal(to_number() + rhs.to_number());
	}

	string ret(std::get<string>(data));
	ret += rhs.to_string();

	return new Literal(ret);
}

Literal Literal::operator-(const Literal& rhs) const {
	return new Literal(to_number() - rhs.to_number());
}

Literal Literal::operator*(const Literal& rhs) const {
	if (!is_string()) {
		return new Literal(to_number() * rhs.to_number());
	}

	number rhs_num = rhs.to_number();

	if (rhs_num < 0) {
		throw "cannot duplicate by a negative number";
	}

	const string& str = std::get<string>(data);
	string ret(str.length() * (size_t) rhs_num, '\0');

	for (auto i = 0; i < rhs_num; ++i) {
		ret += str;
	}

	return new Literal(ret);
}

struct ZeroDivisionError : std::exception {
  const char* what() const noexcept {
  	return "cannot divide by zero!\n";
  }
};

Literal Literal::operator/(const Literal& rhs) const {
	auto rhs_num = rhs.to_number();

	if (rhs_num == 0) {
		throw ZeroDivisionError();
	}

	return new Literal(to_number() / rhs_num);
}

Literal Literal::operator%(const Literal& rhs) const {
	auto rhs_num = rhs.to_number();

	if (rhs_num == 0) {
		throw ZeroDivisionError();
	}

	return new Literal(to_number() % rhs_num);
}

Literal Literal::pow(const Literal& rhs) const {
	number ret = 1;
	number base = to_number();
	number exp = rhs.to_number();

	for (; exp; --exp) {
		ret *= base;
	}

	return new Literal(ret);
}

bool Literal::operator==(const Literal& rhs) const {
	if (data.index() != rhs.data.index()) {
		return false;
	}

	return std::visit(overload {
		[](const null&) { return true; },
		[&](const bool& boolean) { return boolean == std::get<bool>(rhs.data); },
		[&](const number& num) { return num == std::get<number>(rhs.data); },
		[&](const string& str) { return str == std::get<string>(rhs.data); }
	}, data);
}

int Literal::cmp(const Literal& rhs) const {
	if (!is_string()) {
		auto this_num = to_number();
		auto rhs_num = rhs.to_number();

		return this_num < rhs_num ? -1 : this_num > rhs_num ? 1 : 0;
	}

	const string& this_string = std::get<string>(data);
	string rhs_string = rhs.to_string();


	auto pair = std::mismatch(
		this_string.cbegin(), this_string.cend(),
		rhs_string.cbegin(), rhs_string.cend()
	);

	return pair.first < pair.second ? -1 : pair.first > pair.second ? 1 : 0;
}

bool Literal::operator<(const Literal& rhs) const {
	return cmp(rhs) < 0;
}

bool Literal::operator>(const Literal& rhs) const {
	return cmp(rhs) > 0;
}
