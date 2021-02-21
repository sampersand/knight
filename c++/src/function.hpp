#pragma once

#include "value.hpp"
#include <vector>

namespace kn {
	using args_t = std::vector<SharedValue const>;
	using funcptr_t = SharedValue(*)(args_t const&);

	class Function : public Value {
		funcptr_t const func;
		args_t const args;

		Function(funcptr_t name, args_t args);
	public:
		Function() = delete;

		SharedValue run() const override;
		bool operator==(Value const& rhs) const override;

		static SharedValue parse(std::string_view& view);

		static void register_function(char name, size_t arity, funcptr_t func);
		static void initialize();
	};
}
