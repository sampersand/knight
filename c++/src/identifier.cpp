#include "identifier.hpp"
#include "literal.hpp"
#include <unordered_map>

using namespace kn;

Identifier::Identifier(std::string name) noexcept : name(name) {}

// The list of all known variables.
static std::unordered_map<std::string, SharedValue> ENVIRONMENT;

SharedValue Identifier::parse(std::string_view& view) {
	char front = view.front();

	if (!std::islower(front) && front != '_') {
		return nullptr;
	}

	auto start = view.cbegin();

	do {
		view.remove_prefix(1);
		front = view.front();
	} while (std::islower(front) || front == '_' || std::isdigit(front));

	std::string ret(start, view.cbegin());

	return std::make_shared<Identifier>(Identifier(ret));
}

SharedValue Identifier::run() const {
	if (ENVIRONMENT.count(name) == 0) {
		throw Error("unknown identifier encountered: " + name);
	}

	return ENVIRONMENT[name];
}

SharedValue Identifier::assign(SharedValue value) const {
	value = value->run();

	ENVIRONMENT.insert_or_assign(name, value);

	return value;
}
