#include "value.hpp"
using namespace kn;


value::value(bool boolean) : data(boolean) {}
value::value(integer_t integer) : data(integer) {}
value::value(std::string string) : data(string) {}


// shamelessly copied from the cpp docs.
template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
 
// copied from a dude on discord.
template <class Variant, class... Visitors>
decltype(auto) match (Variant&& variant,Visitors&&...visitors) {
   return std::visit(overloaded{std::forward<Visitors>(visitors)...},std::forward<Variant>(variant));
}

bool value::to_boolean() const {
	return match(data,
		[](bool boolean) { return boolean; },
		[](integer_t integer) { return integer != 0; },
		[](std::string string) { return !string.empty(); }
	);
}

// namespace kn {E
// 	typedef intmax_t integer_t;

// 	class value {
// 		std::variant<bool, integer_t, std::string> data;
// 	public:
// 		value(bool boolean);
// 		value(integer_t integer);
// 		value(std::string string);

// 		bool to_boolean(void) const;
// 		integer_t to_integer(void) const;
// 		std::string to_string(void) const;


// 		// value operator+(const value& rhs) const  {
// 		// 	return integer(to_integer() + rhs.to_integer());
// 		// }
// 	};
// }
