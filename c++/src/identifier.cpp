#include "identifier.hpp"
#include "literal.hpp"
#include <unordered_map>

using namespace kn;

Identifier::Identifier(std::string name) noexcept : name(name) {}

UnknownIdentifier::UnknownIdentifier(std::string ident)
	: std::runtime_error("invalid identifier given"), ident(ident)
	{ }

static std::unordered_map<std::string, SharedValue> ENVIRONMENT;

SharedValue Identifier::parse(std::string_view& view) {
	if (!islower(view.front()) && view.front() != '_') {
		return nullptr;
	}

	auto start = view.cbegin();

	do {
		view.remove_prefix(1);
	} while (islower(view.front()) || view.front() == '_' || isdigit(view.front()));

	std::string ret(start, view.cbegin());

	return std::make_shared<Identifier>(Identifier(ret));
}

SharedValue Identifier::run() const {
	if (ENVIRONMENT.count(name) == 0) {
		throw UnknownIdentifier(name);
	}

	return ENVIRONMENT[name];
}

void Identifier::assign(SharedValue value) const {
	ENVIRONMENT.emplace(name, std::move(value));
}
