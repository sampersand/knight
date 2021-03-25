#include "variable.hpp"
#include "literal.hpp"
#include "robin_hood_map.hpp"
#include <iostream>
#include <memory>

using namespace kn;

Variable::Variable(std::string name) noexcept : name(name), assigned(false) {}

// The set of all known variables.
static robin_hood::unordered_map<std::string_view, std::shared_ptr<Variable>> ENVIRONMENT;

SharedValue Variable::parse(std::string_view& view) {
	char front = view.front();

	if (!std::islower(front) && front != '_') {
		return nullptr;
	}

	auto start = view.cbegin();

	do {
		view.remove_prefix(1);
		front = view.front();
	} while (std::islower(front) || front == '_' || std::isdigit(front));

	auto identifier = std::string_view(start, view.cbegin() - start);

	if (auto match = ENVIRONMENT.find(identifier); match != ENVIRONMENT.cend()) {
		return match->second;
	}

	auto variable = std::make_shared<Variable>(std::string(identifier));

	return ENVIRONMENT.emplace(std::string_view(variable->name), std::move(variable)).first->second;
}

std::string Variable::dump() const {
	return "Identifier(" + name + ")";
}

SharedValue Variable::run() const {
	if (assigned) {
		return value;
	} else {
		throw Error("unknown variable encountered: " + name);
	}
}

void Variable::assign(SharedValue newvalue) const noexcept {
	value = newvalue;
	assigned = true;
}
