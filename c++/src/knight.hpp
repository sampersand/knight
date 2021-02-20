#include "value.hpp"

namespace kn {
	shared_value

	std::string str("abci123AA1");
	

	auto begin = str.cbegin();
	auto ident = kn::Identifier::parse1(begin, str.cend());

	std::cout << ((kn::Identifier *) &*ident)->name << "[" << *begin << std::endl;
}
