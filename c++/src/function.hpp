#pragma once

#include "value.hpp"
#include <vector>

namespace kn {
	using funcptr_t = std::shared_ptr<Value>(*)(std::vector<std::shared_ptr<Value>> const&);

	class Function : public Value {
		funcptr_t const func;
		std::vector<std::shared_ptr<Value>> const args;
	public:
		Function(char name, std::vector<std::shared_ptr<Value>> args);

		static std::shared_ptr<Value const> parse(std::string_view& view) override;
		std::shared_ptr<Value const> run() const override;

		static void register_function(char name, size_t arity, funcptr_t func);
		static void initialize();
	};
}
