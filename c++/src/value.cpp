#include "value.hpp"
#include "identifier.hpp"

using namespace kn;

void Value::assign(std::shared_ptr<Value> value) const {
	Identifier(to_string()).assign(value);
}
