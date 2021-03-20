#include "value.hpp"
#include "variable.hpp"
#include "function.hpp"
#include "literal.hpp"

using namespace kn;

static void remove_whitespace(std::string_view &view) {
	while (true) {
		// note that in Knight, all forms of parens and `:` are considered whitespace.
		switch (view.front()) {
			case ' ': case '\t': case '\n': case '\r': case '\v': case '\f':
			case ':':
			case '(': case ')':
			case '[': case ']':
			case '{': case '}':
				view.remove_prefix(1);
				continue;

			case '#':
				do {
					view.remove_prefix(1);
				} while (!view.empty() && view.front() != '\n');

				continue;
			default:
				return;
		}
	}
}

SharedValue Value::parse(std::string_view& view) {
	SharedValue ret;

	remove_whitespace(view);

	if ((ret = Literal::parse(view)) || (ret = Variable::parse(view)) || (ret = Function::parse(view))) {
		return ret;
	} else if (view.empty()) {
		throw std::invalid_argument("nothing to parse");
	} else {
		throw std::invalid_argument("invalid character encountered: " + std::to_string(view.front()));
	}
}

bool Value::to_boolean() const {
	return run()->to_boolean();
}

number Value::to_number() const {
	return run()->to_number();
}

string Value::to_string() const {
	return run()->to_string();
}

SharedValue Value::operator+(Value const& rhs) const {
	return *run() + *rhs.run();
}

SharedValue Value::operator-(Value const& rhs) const {
	return *run() - *rhs.run();
}

SharedValue Value::operator*(Value const& rhs) const {
	return *run() * *rhs.run();
}

SharedValue Value::operator/(Value const& rhs) const {
	return *run() / *rhs.run();
}

SharedValue Value::operator%(Value const& rhs) const {
	return *run() % *rhs.run();
}

SharedValue Value::pow(Value const& rhs) const {
	return run()->pow(*rhs.run());
}

bool Value::operator==(Value const& rhs) const {
	return *run() == *rhs.run();
}

bool Value::operator<(Value const& rhs) const {
	return *run() < *rhs.run();
}

bool Value::operator>(Value const& rhs) const {
	return *run() > *rhs.run();
}

