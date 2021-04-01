#include "variable.hpp"
#include <unordered_map>
#include <iostream>
#include <memory>

using namespace kn;

Variable::Variable(std::string name) noexcept : name(name) {}

// The set of all known variables.
static std::unordered_map<std::string_view, std::shared_ptr<Variable>> ENVIRONMENT;

std::optional<Value> Variable::parse(std::string_view& view) {
	char front = view.front();

	if (!std::islower(front) && front != '_')
		return std::nullopt;

	auto start = view.cbegin();

	do {
		view.remove_prefix(1);
		front = view.front();
	} while (std::islower(front) || front == '_' || std::isdigit(front));

	auto identifier = std::string_view(start, view.cbegin() - start);

	if (auto match = ENVIRONMENT.find(identifier); match != ENVIRONMENT.cend())
		return std::make_optional<Value>(match->second);

	auto variable = std::make_shared<Variable>(std::string(identifier));
	auto result = ENVIRONMENT.emplace(std::string_view(variable->name), std::move(variable));

	return std::make_optional<Value>(result.first->second);
}

std::ostream& Variable::dump(std::ostream& out) const noexcept {
	return out << "Variable(" << name << ")";
}

Value Variable::run() {
	if (!value)
		throw Error("unknown variable encountered: " + name);

	return *value;
}

void Variable::assign(Value newvalue) noexcept {
	value = std::move(newvalue);
}
