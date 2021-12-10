#pragma once
#include <exception>
#include <memory>
#include <string>
#include <sstream>
#include <var.hpp>

class except : public std::exception {
	const std::string _message;

public:
	except(auto&& message) : _message{ std::forward<decltype(message)>(message) } {}
	const char* what() const noexcept override;
};

template<var::Streamable... VT> inline _CONSTEXPR except make_exception(VT const&... message)
{
	std::stringstream ss;
	(ss << ... << message);
	return ss.str().c_str();
}