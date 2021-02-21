#include "value.hpp"

namespace kn {
	void initialize();

	template<typename T>
	SharedValue run(T input) {
		std::string_view view(input);

		return Value::parse(view)->run();
	}
}
