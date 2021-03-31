#pragma once

#include "value.hpp"
#include "small_vector.hpp"

namespace kn {
	// The argument type that functions must accept.
	using args_t = itlib::small_vector<SharedValue, 4, 4>;

	// The pointer type that all functions must fulfill.
	using funcptr_t = SharedValue(*)(args_t const&);

	// The class that represents a function and its arguments within Knight.
	class Function : public Value {
		// A pointer to the function associated with this class.
		funcptr_t const func;

		// The name of the function; used only within `DUMP`.
		char const name;

		// The unevaluated arguments associated with this function.
		args_t const args;

		// Creates a function with the given function and arguments.
		//
		// This is private because the only way to create a `Function` is through `parse`.
		Function(funcptr_t func, char name, args_t args);

	public:

		// You cannot default construct Functions--you must use `parse`.
		Function() = delete;

		// Executes this function, returning the result of the execution.
		SharedValue run() const override;

		// Returns debugging information about this type.
		std::string dump() const override;

		// Attempts to parse a `Function` instance from the `string_view`.
		//
		// If the first character of `view` isn't a known `Function` name, `nullptr` is returned.
		static SharedValue parse(std::string_view& view);

		// Registers a new funciton with the given name, arity, and function pointer.
		//
		// Any previous function associated with `name` will be silently discarded.
		static void register_function(char name, size_t arity, funcptr_t func);

		// Registers all builtin functions.
		static void initialize();
	};
}
