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

Function::Function(funcptr_t func, char name, args_t args) : func(func), name(name), args(args) { }

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
		while (isupper(view.front()) || view.front() == '_') {
			view.remove_prefix(1);
		}
	}

	// parse the arguments out.
	args_t args;
	for(size_t i = 0; i < func_pair.second; ++i) {
		args.push_back(Value::parse(view));
	}

	return std::make_shared<Function>(Function(func_pair.first, front, args));
}


std::string Function::dump() const {
	std::string ret("Function(");

	ret += name;
	for (auto arg : args) {
		ret += ", ";
		ret += arg->dump();
	}

	return ret + ")";
}

void Function::register_function(char name, size_t arity, funcptr_t func) {
	FUNCTIONS.insert(std::make_pair(name, std::make_pair(func, arity)));
}

#define REG_FUNC(name, len, body) Function::register_function(name, len, [](args_t const& args) -> SharedValue body);

// Prompts for a single line from stdin.
static SharedValue prompt(args_t const& args) {
	(void) args;

	string line;
	std::getline(std::cin, line);

	return std::make_shared<Literal>(line);
}

// Gets a random number.
static SharedValue random(args_t const& args) {
	(void) args;

	return std::make_shared<Literal>((number) rand());
}

// Creates a block of code.
static SharedValue block(args_t const& args) {
	return args[0];
}

// Calls a block of code.
static SharedValue call(args_t const& args) {
	return args[0]->run()->run();
}

// Evaluates the argument as Knight source code.
static SharedValue eval(args_t const& args) {
	return kn::run(args[0]->to_string());
}

// Runs a shell command, returns the stdout of the command.
// effectively copied my C impl...
static SharedValue system(args_t const& args) {
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
}

// Stops the program with the given status code.
static SharedValue quit(args_t const& args) {
	exit(args[0]->to_number());
}

// Logical negation of its argument.
static SharedValue not_(args_t const& args) {
	return std::make_shared<Literal>((bool) !args[0]->to_boolean());
}

// Returns the length of the argument, when converted to a string.
static SharedValue length(args_t const& args) {
	return std::make_shared<Literal>((number) args[0]->to_string().length());
}

// Returns the length of the argument, when converted to a string.
static SharedValue dump(args_t const& args) {
	auto arg = args[0]->run();

	std::cout << arg->dump();

	return arg;
}

// Runs the value, then converts it to a string and prints it. The execution result is returned.
//
// If the string ends with a backslash, its removed before printing. Otherwise, a newline is added.
static SharedValue output(args_t const& args) {
	auto str = args[0]->to_string();

	if (!str.empty() && str.back() == '\\') {
		str.pop_back(); // delete the trailing newline

		std::cout << str;
	} else {
		std::cout << str << std::endl;
	}

	return std::make_shared<Literal>();
}

// Adds two values together.
static SharedValue add(args_t const& args) {
	return *args[0] + *args[1];
}

// Subtracts the second value from the first.
static SharedValue sub(args_t const& args) {
	return *args[0] - *args[1];
}

// Multiplies the two values together.
static SharedValue mul(args_t const& args) {
	return *args[0] * *args[1];
}
// Divides the first value by the second.
static SharedValue div(args_t const& args) {
	return *args[0] / *args[1];
}

// Modulos the first value by the second.
static SharedValue mod(args_t const& args) {
	return *args[0] % *args[1];
}

// Raises the first value to the power of the second.
static SharedValue pow(args_t const& args) {
	return args[0]->pow(*args[1]);
}

// Checks to see if the two values are equal.
static SharedValue eql(args_t const& args) {
	return std::make_shared<Literal>(*args[0]->run() == *args[1]->run());
}	

// Checks to see if the first value is less than the second.
static SharedValue lth(args_t const& args) {
	return std::make_shared<Literal>(*args[0]->run() <  *args[1]->run());
}

// Checks to see if the first value is greater than the second.
static SharedValue gth(args_t const& args) {
	return std::make_shared<Literal>(*args[0]->run() >  *args[1]->run());
}

// Evaluates the first value, returning it if it's falsey. Otherwise evaluates and returns the second.
static SharedValue and_(args_t const& args) {
	auto lhs = args[0]->run();

	return lhs->to_boolean() ? args[1]->run() : lhs;
}

// Evaluates the first value, returning it if it's truthy. Otherwise evaluates and returns the second.
static SharedValue or_(args_t const& args) {
	auto lhs = args[0]->run();

	return lhs->to_boolean() ? lhs : args[1]->run();
}

// Runs the first value, then runs the second and returns it.
static SharedValue then(args_t const& args) {
	args[0]->run();

	return args[1]->run();
}

// Assigns the second value to the first.
static SharedValue assign(args_t const& args) {
	return args[0]->assign(args[1]);
}

// Evaluates the second value while the first one is truthy.
//
// The last value the body returned will be returned. If the body never ran, null will be returned.
static SharedValue while_(args_t const& args) {
	while (args[0]->to_boolean()) {
		args[1]->run();
	}

	return std::make_shared<Literal>();
}

// Runs the second value if the first is truthy. Otherwise, runs the third value.
static SharedValue if_(args_t const& args) {
	return args[0]->to_boolean() ? args[1]->run() : args[2]->run();
}

// Returns a substring of the first value, with the second value as the start index and the third as the length.
//
// If the length is out of bounds, it's assumed to be the string length.
static SharedValue get(args_t const& args) {
	auto str = args[0]->to_string();
	auto start = args[1]->to_number();
		auto length = args[2]->to_number();

		if (start >= (number) str.length()) {
			return std::make_shared<Literal>(string());
		}
		return std::make_shared<Literal>(str.substr(start, length));
	}

// Returns a new string with first string's range `[second, second+third)` replaced by the fourth value.
static SharedValue substitute(args_t const& args) {
	auto str = args[0]->to_string();
	auto start = args[1]->to_number();
	auto length = args[2]->to_number();
	auto repl = args[3]->to_string();

	// this could be made more efficient by preallocating memory
	return std::make_shared<Literal>(str.substr(0, start) + repl + str.substr(start + length));
}


void Function::initialize(void) {
	srand(time(NULL)); // seed `R`'s random number.

	Function::register_function('P', 0, &::prompt);
	Function::register_function('R', 0, &::random);

	Function::register_function('B', 1, &::block);
	Function::register_function('C', 1, &::call);
	Function::register_function('E', 1, &::eval);

	Function::register_function('`', 1, &::system);
	Function::register_function('Q', 1, &::quit);
	Function::register_function('!', 1, &::not_);
	Function::register_function('L', 1, &::length);
	Function::register_function('D', 1, &::dump);
	Function::register_function('O', 1, &::output);
	Function::register_function('+', 2, &::add);
	Function::register_function('-', 2, &::sub);
	Function::register_function('*', 2, &::mul);
	Function::register_function('/', 2, &::div);
	Function::register_function('%', 2, &::mod);
	Function::register_function('^', 2, &::pow);
	Function::register_function('?', 2, &::eql);
	Function::register_function('<', 2, &::lth);
	Function::register_function('>', 2, &::gth);
	Function::register_function('&', 2, &::and_);
	Function::register_function('|', 2, &::or_);
	Function::register_function(';', 2, &::then);
	Function::register_function('=', 2, &::assign);
	Function::register_function('W', 2, &::while_);

	Function::register_function('I', 3, &::if_);
	Function::register_function('G', 3, &::get);

	Function::register_function('S', 4, &::substitute);
}
