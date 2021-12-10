#include "make_exception.hpp"

const char* except::what() const noexcept { return _message.c_str(); }