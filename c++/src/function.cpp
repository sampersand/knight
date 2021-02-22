#include "function.hpp"
#include "value.hpp"
#include "literal.hpp"
#include "knight.hpp"

#include <unordered_map>
#include <iostream>
#include <cstdio>

using namespace kn;

// The list of all _functions_
static std::unordered_map<char, std::pair<funcptr_t, size_t>> FUNCTIONS;

Function::Function(funcptr_t func, args_t args) : func(func), args(args) { }

SharedValue Function::run() const {
	return func(args);
}

bool Function::operator==(Value const& rhs) const {
	return this == &rhs;
}

SharedValue Function::parse(std::string_view& view) {
	char front = view.front();

	// if the first character isn't a valid function identifier, then just return early.
	if (FUNCTIONS.count(front) == 0) {
		return nullptr;
	}

	view.remove_prefix(1);

	auto func_pair = FUNCTIONS[front];

	// remove trailing upper-case letters for keyword functions.
	if (isupper(front)) {
		while (isupper(view.front())) {
			view.remove_prefix(1);
		}
	}

	// parse the arguments out.
	args_t args;
	for(size_t i = 0; i < func_pair.second; ++i) {
		args.push_back(Value::parse(view));
	}

	return std::make_shared<Function>(Function(func_pair.first, args));
}

void Function::register_function(char name, size_t arity, funcptr_t func) {
	FUNCTIONS.insert(std::make_pair(name, std::make_pair(func, arity)));
}

#define DECL_FUNC(name, len, body) register_function(name, len, [](args_t const& args) -> SharedValue body);

void Function::initialize(void) {
	srand(time(NULL)); // seed `R`'s random number.

	// Prompts for a single line from stdin.
	DECL_FUNC('P', 0, {
		(void) args;

		string line;
		std::getline(std::cin, line);

		return std::make_shared<Literal>(line);
	});

	// Gets a random number.
	DECL_FUNC('R', 0, {
		(void) args;

		return std::make_shared<Literal>((number) rand());
	});

	// Creates a block of code.
	DECL_FUNC('B', 1, {
		return args[0];
	});

	// Calls a block of code.
	DECL_FUNC('C', 1, {
		return args[0]->run()->run();
	});

	// Evaluates the argument as Knight source code.
	DECL_FUNC('E', 1, {
		return kn::run(args[0]->to_string());
	});

	// Runs a shell command, returns the stdout of the command.
	// effectively copied my C impl...
	DECL_FUNC('`', 1, {
		auto cmd = args[0]->to_string();

		FILE *stream = popen(cmd.c_str(), "r");

		if (stream == NULL) {
			throw Error("unable to execute command.");
		}

		size_t cap = 2048;
		size_t length = 0;
		size_t tmp;

		char *result = (char *) malloc(cap);
		if (result == NULL) {
			throw Error("cant malloc");
		}

		while (0 != (tmp = fread((char *) (((size_t) result) + length), 1, cap - length, stream))) {
			length += tmp;
			if (length == cap) {
				cap *= 2;
				result = (char *) realloc(result, cap);
				if (result == NULL) {
					throw Error("cannot realloc");
				}
			}
		}

		// ignore any errors with the es
		if (ferror(stream)) {
			throw Error("unable to read command stream");
		}

		if (pclose(stream) == -1) {
			throw Error("unable to close command stream.");
		}

		return std::make_shared<Literal>(string(result));
	});

	// Stops the program with the given status code.
	DECL_FUNC('Q', 1, {
		exit(args[0]->to_number());
	});

	// Logical negation of its argument.
	DECL_FUNC('!', 1, {
		return std::make_shared<Literal>((bool) !args[0]->to_boolean());
	});

	// Returns the length of the argument, when converted to a string.
	DECL_FUNC('L', 1, {
		return std::make_shared<Literal>((number) args[0]->to_string().length());
	});

	// Runs the value, then converts it to a string and prints it. The execution result is returned.
	//
	// If the string ends with a backslash, its removed before printing. Otherwise, a newline is added.
	DECL_FUNC('O', 1, {
		auto value = args[0]->run();
		auto str = value->to_string();

		if (!str.empty() && str.back() == '\\') {
			str.pop_back(); // delete the trailing newline

			std::cout << str;
		} else {
			std::cout << str << std::endl;
		}

		return value;
	});

	// Adds two values together.
	DECL_FUNC('+', 2, {
		return *args[0] + *args[1];
	});

	// Subtracts the second value from the first.
	DECL_FUNC('-', 2, {
		return *args[0] - *args[1];
	});

	// Multiplies the two values together.
	DECL_FUNC('*', 2, {
		return *args[0] * *args[1];
	});

	// Divides the first value by the second.
	DECL_FUNC('/', 2, {
		return *args[0] / *args[1];
	});

	// Modulos the first value by the second.
	DECL_FUNC('%', 2, {
		return *args[0] % *args[1];
	});

	// Raises the first value to the power of the second.
	DECL_FUNC('^', 2, {
		return args[0]->pow(*args[1]);
	});

	// Checks to see if the two values are equal.
	DECL_FUNC('?', 2, {
		return std::make_shared<Literal>(*args[0] == *args[1]);
	});	

	// Checks to see if the first value is less than the second.
	DECL_FUNC('<', 2, {
		return std::make_shared<Literal>(*args[0] < *args[1]);
	});

	// Checks to see if the first value is greater than the second.
	DECL_FUNC('>', 2, {
		return std::make_shared<Literal>(*args[0] > *args[1]);
	});

	// Evaluates the first value, returning it if it's falsey. Otherwise evaluates and returns the second.
	DECL_FUNC('&', 2, {
		auto lhs = args[0]->run();

		return lhs->to_boolean() ? args[1]->run() : lhs;
	});

	// Evaluates the first value, returning it if it's truthy. Otherwise evaluates and returns the second.
	DECL_FUNC('|', 2, {
		auto lhs = args[0]->run();

		return lhs->to_boolean() ? lhs : args[1]->run();
	});

	// Runs the first value, then runs the second and returns it.
	DECL_FUNC(';', 2, {
		args[0]->run();

		return args[1]->run();
	});

	// Assigns the second value to the first.
	DECL_FUNC('=', 2, {
		return args[0]->assign(args[1]);
	});

	// Evaluates the second value while the first one is truthy.
	//
	// The last value the body returned will be returned. If the body never ran, null will be returned.
	DECL_FUNC('W', 2, {
		SharedValue ret = std::make_shared<Literal>();

		while (args[0]->to_boolean()) {
			ret = args[1]->run();
		}

		return ret;
	});

	// Runs the second value if the first is truthy. Otherwise, runs the third value.
	DECL_FUNC('I', 3, {
		return args[0]->to_boolean() ? args[1]->run() : args[2]->run();
	});

	// Returns a substring of the first value, with the second value as the start index and the third as the length.
	//
	// If the length is out of bounds, it's assumed to be the string length.
	DECL_FUNC('G', 3, {
		auto str = args[0]->to_string();
		auto start = args[1]->to_number();
		auto length = args[2]->to_number();

		if (start >= (number) str.length()) {
			return std::make_shared<Literal>(string());
		}
		return std::make_shared<Literal>(str.substr(start, length));
	});

	// Returns a new string with first string's range `[second, second+third)` replaced by the fourth value.
	DECL_FUNC('S', 4, {
		auto str = args[0]->to_string();
		auto start = args[1]->to_number();
		auto length = args[2]->to_number();
		auto repl = args[3]->to_string();

		// this could be made more efficient by preallocating memory
		return std::make_shared<Literal>(str.substr(0, start) + repl + str.substr(start + length));
	});
}
