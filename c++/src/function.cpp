#include "function.hpp"
#include "value.hpp"
#include "literal.hpp"
#include "knight.hpp"

#include <unordered_map>
#include <iostream>

using namespace kn;
static std::unordered_map<char, std::pair<funcptr_t, size_t>> FUNCTIONS;

Function::Function(funcptr_t func, args_t args) : func(func), args(args) {
}

SharedValue Function::run() const {
	return func(args);
}

bool Function::operator==(Value const& rhs) const {
	return this == &rhs;
}

SharedValue Function::parse(std::string_view& view) {
	char front = view.front();

	if (FUNCTIONS.count(front) == 0) {
		return nullptr;
	}

	view.remove_prefix(1);

	auto func_pair = FUNCTIONS[front];

	if (isupper(front)) {
		while (isupper(view.front())) {
			view.remove_prefix(1);
		}
	}

	args_t args;

	for(size_t i = 0; i < func_pair.second; ++i) {
		args.push_back(Value::parse(view));
	}

	return std::make_shared<Function>(Function(func_pair.first, args));
}

void Function::register_function(char name, size_t arity, funcptr_t func) {
	FUNCTIONS.insert(std::make_pair(name, std::make_pair(func, arity)));
}

#define MAKE_SHARED(x) ((SharedValue) std::make_shared<Literal>(Literal(x)))

void Function::initialize(void) {
	srand(time(NULL));

	register_function('P', 0, [](args_t const&) {
		std::string line;

		std::getline(std::cin, line);

		return MAKE_SHARED(line);
	});

	register_function('R', 0, [](args_t const&) {
		return MAKE_SHARED((number) rand());
	});

	register_function('B', 1, [](args_t const& args) {
		return args[0];
	});

	register_function('C', 1, [](args_t const& args) {
		return args[0]->run()->run();
	});

	register_function('E', 1, [](args_t const& args) {
		return kn::run(args[0]->to_string());
	});

	register_function('`', 1, [](args_t const& args) {
		if (true) throw "todo: exec";
		(void) args;
		return MAKE_SHARED(kn::Literal()); // TODO
	});

	register_function('Q', 1, [](args_t const& args) {
		exit(args[0]->to_number());
		return MAKE_SHARED(kn::Literal()); // todo: remove this.
	});

	register_function('!', 1, [](args_t const& args) {
		return MAKE_SHARED((bool) !args[0]->to_boolean());
	});

	register_function('L', 1, [](args_t const& args) {
		return MAKE_SHARED((number) args[0]->to_string().length());
	});

	register_function('O', 1, [](args_t const& args) {
		auto value = args[0]->run();
		auto str = value->to_string();

		if (!str.empty() && str.back() == '\\') {
			str.pop_back();

			std::cout << str;
		} else {
			std::cout << str << std::endl;
		}

		return value;
	});

	register_function('+', 2, [](args_t const& args) {
		std::cout << args.size() << std::endl;
		return *args[0]->run() + *args[1]->run();
	});

	register_function('-', 2, [](args_t const& args) {
		return *args[0]->run() - *args[1]->run();
	});

	register_function('*', 2, [](args_t const& args) {
		return *args[0]->run() * *args[1]->run();
	});

	register_function('/', 2, [](args_t const& args) {
		return *args[0]->run() / *args[1]->run();
	});

	register_function('%', 2, [](args_t const& args) {
		return *args[0]->run() % *args[1]->run();
	});

	register_function('^', 2, [](args_t const& args) {
		return args[0]->run()->pow(*args[1]->run());
	});

	register_function('?', 2, [](args_t const& args) {
		return MAKE_SHARED(*args[0]->run() == *args[1]->run());
	});

	register_function('<', 2, [](args_t const& args) {
		return MAKE_SHARED(*args[0]->run() <  *args[1]->run());
	});

	register_function('>', 2, [](args_t const& args) {
		return MAKE_SHARED(*args[0]->run() >  *args[1]->run());
	});

	register_function(';', 2, [](args_t const& args) {
		args[0]->run();
		return args[1]->run();
	});

	register_function('=', 2, [](args_t const& args) {
		return args[0]->assign(args[1]);
	});

	register_function('W', 2, [](args_t const& args) {
		auto ret = MAKE_SHARED(kn::Literal());

		while (args[0]->to_boolean()) {
			ret = args[1]->run();
		}

		return ret;
	});

	register_function('&', 2, [](args_t const& args) {
		auto lhs = args[0]->run();

		return lhs->to_boolean() ? args[1]->run() : lhs;
	});

	register_function('|', 2, [](args_t const& args) {
		auto lhs = args[0]->run();

		return lhs->to_boolean() ? lhs : args[1]->run();
	});

	register_function('I', 3, [](args_t const& args) {
		return args[0]->to_boolean() ? args[1]->run() : args[2]->run();
	});

	register_function('G', 3, [](args_t const& args) {
		throw "todo: get";
		(void) args;
		return MAKE_SHARED(kn::Literal()); // todo: remove this.
	});

	register_function('S', 4, [](args_t const& args) {
		throw "todo: set";
		(void) args;
		return MAKE_SHARED(kn::Literal()); // todo: remove this.
	});
}
