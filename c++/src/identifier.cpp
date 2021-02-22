#include "identifier.hpp"
#include "literal.hpp"
#include <unordered_map>

using namespace kn;

Identifier::Identifier(std::string name) : name(name) {}

bool Identifier::to_boolean() const {
	return run().to_boolean();
}

number Identifier::to_number() const {
	return run().to_number();
}

string Identifier::to_string() const {
	return run().to_string();
}

static std::unordered_map<std::string, std::shared_ptr<Value>> ENVIRONMENT;

UnknownIdentifier::UnknownIdentifier(std::string ident) : ident(ident), message("invalid identifier '") {
	message.append(ident);
	message.push_back('\'');
}

const char *UnknownIdentifier::what() const noexcept {
	return message.c_str();
}

Literal Identifier::run() const {
	if (ENVIRONMENT.count(name) == 0) {
		throw UnknownIdentifier(name);
	}

	return std::shared_ptr<Value>(ENVIRONMENT[name]);
}

void Identifier::assign(std::shared_ptr<Value> value) const {
	ENVIRONMENT.emplace(name, std::move(value));
}


// is there a way to change what "this" means? eg instead of taking a reference to `this`, 
