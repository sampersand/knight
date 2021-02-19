#include "identifier.hpp"
#include "literal.hpp"
#include <unordered_map>

using namespace kn;

Identifier::Identifier(std::string name) noexcept : name(name) {}

static std::unordered_map<std::string, std::shared_ptr<Value const>> ENVIRONMENT;

UnknownIdentifier::UnknownIdentifier(std::string ident) : std::runtime_error("invalid identifier given"), ident(ident) {}

std::shared_ptr<Value const> Identifier::parse(std::string_view& view) {
	char x = view[0];
	(void) view;
	return nullptr;
}

std::shared_ptr<Value const> Identifier::run() const {
	if (ENVIRONMENT.count(name) == 0) {
		throw UnknownIdentifier(name);
	}

	return ENVIRONMENT[name];
}

void Identifier::assign(std::shared_ptr<Value const> value) const {
	ENVIRONMENT.emplace(name, std::move(value));
}
