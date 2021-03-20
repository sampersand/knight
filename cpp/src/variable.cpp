#include "variable.hpp"
#include "literal.hpp"
#include <unordered_set>
#include <memory>

using namespace kn;

Variable::Variable(std::string name) noexcept : name(name), assigned(false) {}

bool kn::operator==(Variable const& lhs, Variable const& rhs) {
	return lhs.name == rhs.name;
}

namespace std {
	template <>
	struct hash<Variable> {
		size_t operator()(const Variable& variable) const {
			return hash<std::string>()(variable.name);
		}
	};
}

// The set of all known variables.
static std::unordered_set<std::shared_ptr<Variable>> ENVIRONMENT;

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

	std::string identifier(start, view.cbegin());

	return *ENVIRONMENT.insert(std::make_shared<Variable>(identifier)).first;
}

std::string Variable::dump() const {
	return "Identifier(" + name + ")";
}

SharedValue Variable::run() const {
	printf("%p\n", (void*)this);
	if (assigned) {
		return value;
	} else {
		throw Error("unknown variable encountered: " + name);
	}
}

void Variable::assign(SharedValue newvalue) const noexcept {
	value = newvalue;
	assigned = true;
	printf("%p\n", (void*)this);
}
