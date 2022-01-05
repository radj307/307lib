#pragma once
#include <memory>
#include <string>
#include <sstream>
#include <exception>
#include <var.hpp>

/**
 * @class	except
 * @brief	Exception object with a custom message stored as a member variable.
 */
class except : public std::exception {
	const std::string _message;

public:
	except() = default;
	except(auto&& message) : _message{ std::forward<decltype(message)>(message) } {}
	const char* what() const noexcept override { return _message.c_str(); }
};

/**
 * @brief				Create an exception with a given message.
 * @tparam ReturnT		The type of exception to return.
 * @tparam ...VT		Variadic Templated Types.
 * @param ...message	The message shown when calling the what() function.
 * @returns				ReturnT
 */
template<std::derived_from<std::exception> ReturnT, var::Streamable<std::stringstream>... VT>
WINCONSTEXPR ReturnT make_custom_exception(VT const&... message)
{
	std::stringstream ss;
	(ss << ... << message);
	return{ ss.str() };
}

template<var::Streamable<std::stringstream>... VT>
WINCONSTEXPR except make_exception(VT const&... message) { return make_custom_exception<except>(message...); }
