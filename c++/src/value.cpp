#include "value.hpp"
#include "identifier.hpp"

using namespace kn;

void Value::assign(std::shared_ptr<Value const> value) const {
	Identifier(to_string()).assign(value);
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
