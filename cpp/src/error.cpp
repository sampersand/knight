#include "error.hpp"

kn::Error::Error(std::string const& what_arg) : std::runtime_error(what_arg) {}
