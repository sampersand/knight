#include "value.hpp"
#include "variable.hpp"
#include "function.hpp"

using namespace kn;
using std::make_shared;
using std::shared_ptr;

Value::Value() noexcept : data(null {}) {}
Value::Value(bool boolean) noexcept : data(boolean) {}
Value::Value(number num) noexcept : data(num) {}
Value::Value(string str) noexcept : Value(make_shared<string>(str)) {}
Value::Value(shared_ptr<string> str) noexcept : data(str) {}
Value::Value(shared_ptr<Variable> var) noexcept : data(var) {}
Value::Value(shared_ptr<Function> func) noexcept : data(func) {}

static void remove_keyword(std::string_view& view) {
	do {
			view.remove_prefix(1);
	} while (std::isupper(view.front()) || view.front() == '_');
}

std::optional<Value> Value::parse(std::string_view& view) {
	char front;

top:
	if (view.empty())
		return std::nullopt;

	// note that in knight, all forms of parens and `:` are considered whitespace.
	switch (front = view.front()) {
	case '#':
		do {
			view.remove_prefix(1);
		} while (!view.empty() && view.front() != '\n');
		// fallthrough, as we know the first character is `\n`.

	case ' ': case '\t': case '\n': case '\r': case '\v': case '\f':
	case '(': case  ')': case  '[': case  ']': case  '{': case  '}': case ':': 
		do {
			view.remove_prefix(1);
		} while (std::isspace(view.front()));
		goto top;

	case 'N':
		remove_keyword(view);
		return std::make_optional<Value>();

	case 'T':
	case 'F':
		remove_keyword(view);
		return std::make_optional<Value>(front == 'T');

	case '\'':
	case '\"': {
		view.remove_prefix(1);
		auto begin = view.cbegin();

		for(char quote = front; quote != view.front(); view.remove_prefix(1))
			if (view.empty())
				throw Error("unmatched quote encountered!");

		string ret(begin, view.cbegin());
		view.remove_prefix(1);

		return std::make_optional<Value>(ret);
	}

	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9': {
		number num = 0;

		for (; std::isdigit(front = view.front()); view.remove_prefix(1)) {
			num = num * 10 + (front - '0');
		}

		return std::make_optional<Value>(num);
	}

	default:
		if (auto var = Variable::parse(view))
			return std::make_optional<Value>(*var);

		if (auto func = Function::parse(view))
			return std::make_optional<Value>(*func);

		throw Error("invalid character encountered: " + std::to_string(front));
	}
}


template <typename... Fns>
struct overload : Fns... { using Fns::operator()...; };

template <typename... Fns>
overload(Fns...) -> overload<Fns...>;

bool Value::to_boolean() {
	return std::visit(overload {
		[](null) { return false; },
		[](bool boolean) { return boolean; },
		[](number num) { return num != 0; },
		[](shared_ptr<string> const& str) { return str->length() != 0; },
		[](shared_ptr<Variable> const& var) { return var->run().to_boolean(); },
		[](shared_ptr<Function> const& func) { return func->run().to_boolean(); }
	}, data);
}

number Value::to_number() {
	return std::visit(overload {
		[](null) { return (number) 0; },
		[](bool boolean) { return (number) boolean; },
		[](number num) { return num; },
		[](shared_ptr<string> const& str) {
			// a custom `stroll` that will will just stop at the first invalid character
			number ret = 0;
			auto begin = std::find_if_not(str->cbegin(), str->cend(), [](char c) { return std::isspace(c); });

			if (begin == str->cend())
				return (number) 0;

			int sign = (*begin == '-') ? -1 : 1;

			if (*begin == '-' || *begin == '+')
				++begin;

			for (; begin != str->cend() && std::isdigit(*begin); ++begin)
				ret = ret * 10 + (*begin - '0');

			return ret * sign;
		},
		[](shared_ptr<Variable> const& var) { return var->run().to_number(); },
		[](shared_ptr<Function> const& func) { return func->run().to_number(); },
	}, data);
}


shared_ptr<string> Value::to_string() {
	return std::visit(overload {
		[](null) {
			static shared_ptr<string> null_string = make_shared<string>("null");
			return null_string;
		},
		[](bool boolean) {
			static shared_ptr<string> true_string = make_shared<string>("true");
			static shared_ptr<string> false_string = make_shared<string>("false");
			return boolean ? true_string : false_string;
		},
		[](number num) { return make_shared<string>(std::to_string(num)); },
		[](shared_ptr<string> const& str) { return str; },
		[](shared_ptr<Variable> const& var) { return var->run().to_string(); },
		[](shared_ptr<Function> const& func) { return func->run().to_string(); },
	}, data);
}

shared_ptr<Variable> Value::as_variable() const {
	if (auto var = std::get_if<shared_ptr<Variable>>(&data))
		return *var;

	throw Error("invalid kind for 'as_variable'");
}

std::ostream& Value::dump(std::ostream& out) const {
	std::visit(overload {
		[&](null) { out << "Null()"; }, 
		[&](bool boolean) { out << (boolean ? "Boolean(true)" : "Boolean(false)"); },
		[&](number num) { out << "Number(" << num << ")"; },
		[&](shared_ptr<string> const& str) { out << "String(" << *str << ")"; },
		[&](shared_ptr<Variable> const& var) { out << var; },
		[&](shared_ptr<Function> const& func) { out << func; },
	}, data);

	return out;
}

Value Value::run() {
	if (auto var = std::get_if<shared_ptr<Variable>>(&data))
		return (*var)->run();

	if (auto func = std::get_if<shared_ptr<Function>>(&data))
		return (*func)->run();

	return *this;
}

Value Value::operator+(Value&& rhs) {
	if (auto str = std::get_if<shared_ptr<string>>(&data))
		return Value(make_shared<string>(**str + *rhs.to_string()));

	if (auto num = std::get_if<number>(&data))
		return Value(*num + rhs.to_number());

	throw Error("invalid kind given to '+'");
}

Value Value::operator-(Value&& rhs) {
	if (auto num = std::get_if<number>(&data))
		return Value(*num - rhs.to_number());

	throw Error("invalid kind given to '-'");
}

Value Value::operator*(Value&& rhs) {
	if (auto num = std::get_if<number>(&data))
		return Value(*num * rhs.to_number());

	auto str = std::get_if<shared_ptr<string>>(&data);

	if (!str)
		throw Error("invalid kind given to '*'");

	number rhs_num = rhs.to_number();

	if (rhs_num < 0)
		throw Error("cannot duplicate by a negative number");

	string ret;

	for (auto i = 0; i < rhs_num; ++i)
		ret += **str;

	return Value(ret);
}

Value Value::operator/(Value&& rhs) {
	number num;

	if (auto pnum = std::get_if<number>(&data))
		num = *pnum;
	else
		throw Error("invalid kind given to '/'");

	auto rnum = rhs.to_number();

	if (!rnum)
		throw new Error("Cannot divide by zero");

	return Value(num / rnum);
}

Value Value::operator%(Value&& rhs) {
	number num;

	if (auto pnum = std::get_if<number>(&data))
		num = *pnum;
	else
		throw Error("invalid kind given to '%'");

	auto rnum = rhs.to_number();

	if (!rnum)
		throw new Error("Cannot modulo by zero");

	return Value(num % rnum);
}

Value Value::pow(Value&& rhs) {
	number base;

	if (auto pbase = std::get_if<number>(&data))
		base = *pbase;
	else
		throw Error("invalid kind given to '%'");

	number exp = rhs.to_number();
	number ret;

	if (base == 1) ret = 1;
	else if (base == -1) ret = exp & 1 ? -1 : 1;
	else if (exp == 1) ret = base;
	else if (exp == 0) ret = 1;
	else if (exp < 0) ret = 0; // already handled the `base == -1` case
	else {
		ret = 1;

		for (; exp > 0; --exp) ret *= base;
	}
	return Value(ret);
}

bool Value::operator==(Value&& rhs) {
	if (data.index() != rhs.data.index())
		return false;

	return std::visit(overload {
		[&](null) { return true; },
		[&](bool boolean) { return boolean == std::get<bool>(rhs.data); },
		[&](number num) { return num == std::get<number>(rhs.data); },
		[&](shared_ptr<string> const& str) { return *str == *std::get<shared_ptr<string>>(rhs.data); },
		[&](shared_ptr<Variable> const& var) { return var == std::get<shared_ptr<Variable>>(rhs.data); },
		[&](shared_ptr<Function> const& var) { return var == std::get<shared_ptr<Function>>(rhs.data); }
	}, data);
}

bool Value::operator<(Value&& rhs) {
	return std::visit([&](auto&& lhs) -> bool {
		using T = std::decay_t<decltype(lhs)>;

		if constexpr (std::is_same_v<T, number>) return lhs < rhs.to_number();
		else if constexpr (std::is_same_v<T, shared_ptr<string>>) return *lhs < *rhs.to_string();
		else if constexpr (std::is_same_v<T, bool>) return rhs.to_boolean() && !lhs;
		else throw Error("invalid kind given to '<'");
	}, data);
}

bool Value::operator>(Value&& rhs) {
	return std::visit([&](auto&& lhs) -> bool {
		using T = std::decay_t<decltype(lhs)>;

		if constexpr (std::is_same_v<T, number>) return lhs > rhs.to_number();
		else if constexpr (std::is_same_v<T, shared_ptr<string>>) return *lhs > *rhs.to_string();
		else if constexpr (std::is_same_v<T, bool>) return !rhs.to_boolean() && lhs;
		else throw Error("invalid kind given to '>'");
	}, data);
}
