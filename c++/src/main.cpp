#include "knight.hpp"
#include <iostream>

using namespace kn;

void usage(char const* program) {
	std::cerr << "usage: " << program << " (-e 'expression' | -f file)" << std::endl;
}

int main(int argc, char **argv) {
	(void) argc;
	(void) argv;
	// if (argc != 3) {
	// 	goto usage;
	// }

	// std::string

	// kn::initialize();

	// if (strcmp(argv[1], "-e") == 0)  {

	// 	usage(argv[0]);
	// }
	// switch(argv[1]) {
	// 	case 
	// }

	initialize();

	try {
		run("O + 2 5");
	} catch (char const* err) {
		std::cout << err << std::endl;
	}
	// // std::string str("abci123AA1");
	// // std::string str("P");
	// // auto view = std::string_view(str);

	// kn::Function func('R', kn::args_t());
	// std::cout << func.run()->to_string() << std::endl;
	// // auto input = kn::Identifier::parse("Pabc");

	// // auto begin = str.cbegin();
	// // auto ident = kn::Identifier::parse1(begin, str.cend());

	// // std::cout << ((kn::Identifier *) &*ident)->name << "[" << *begin << std::endl;
}
