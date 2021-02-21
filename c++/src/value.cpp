#include "value.hpp"
#include "identifier.hpp"
#include "function.hpp"
#include "literal.hpp"

using namespace kn;

SharedValue Value::parse(std::string_view& view) {
	remove_whitespace:
	switch (view.front()) {
		case ' ': case '\t': case '\n': case '\r': case '\v': case '\f':
		case ':':
		case '(': case ')':
		case '[': case ']':
		case '{': case '}':
			view.remove_prefix(1);
			goto remove_whitespace;

		case '#':
			do {
				view.remove_prefix(1);
			} while (!view.end() && view.front() != '\n');

			goto remove_whitespace;
	}

	SharedValue ret;

	if ((ret = Literal::parse(view)) || (ret = Identifier::parse(view)) || (ret = Function::parse(view))) {
		return ret;
	} else {
		throw std::invalid_argument("invalid character encountered: " + std::to_string(view.front()));
	}
}

SharedValue Value::assign(std::shared_ptr<Value const> value) const {
	return Identifier(to_string()).assign(value);
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

