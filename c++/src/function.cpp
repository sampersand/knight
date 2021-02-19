#include "function.hpp"
#include "value.hpp"
#include "literal.hpp"
#include <unordered_map>

using namespace kn;

struct func_t {
	funcptr_t ptr;
	size_t arity;
};

static std::unordered_map<char, func_t> FUNCTIONS;

static inline funcptr_t ptr_for(char name) {
	if (FUNCTIONS.count(name) == 0) {
		throw std::invalid_argument("invalid function name given.");
	} else {
		return FUNCTIONS[name].ptr;
	}
}

Function::Function(char name, std::vector<std::shared_ptr<Value>> args) : func(ptr_for(name)), args(args) {
	assert(FUNCTIONS[name].arity == args.size()); // make sure the arity matches.
}

std::shared_ptr<Value const> Function::run() const {
	return func(args);
}

std::shared_ptr<Value const> Function::parse(std::string_view& view) {
	(void) view;
	return nullptr;
}

void Function::register_function(char name, size_t arity, funcptr_t func) {
	FUNCTIONS.insert({name, { func, arity }});
}


std::shared_ptr<Value> doit(std::vector<std::shared_ptr<Value>>const &) {
	return std::make_shared<Literal>(Literal(rand()));
}

std::shared_ptr<Value> add(std::vector<std::shared_ptr<Value>> const& ) {
	// return args[0] + args[1];
	return std::make_shared<Literal>(Literal(rand()));
}

void Function::initialize(void) {
	srand(time(nullptr));

	register_function('P', 0, &doit);
	// register_function('P', 0, [](std::vector<std::shared_ptr<Value>>&){ return shared_ptr(Literal(rand())); })
}

