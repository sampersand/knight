#pragma once

#include "value.hpp"
#include <vector>

namespace kn {
	using funcptr_t = SharedValue(*)(std::vector<SharedValue const> const&);

	class Function : public Value {
		funcptr_t const func;
		std::vector<SharedValue const> const args;
	public:
		Function(char name, std::vector<SharedValue const> const args);

		SharedValue run() const override;
		static SharedValue parse(std::string_view& view) override;

		static void register_function(char name, size_t arity, funcptr_t func);
		static void initialize();
	};
}
