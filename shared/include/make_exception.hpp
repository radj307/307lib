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
 * @brief				Create a custom exception type with the given arguments as a message.
 * @tparam ReturnT		The type of exception to return.
 * @tparam ...VT		Variadic Templated Types.
 * @param ...message	The message shown when calling the what() function.
 * @returns				ReturnT
 */
template<std::derived_from<std::exception> ReturnT, var::Streamable<std::stringstream>... Ts>
WINCONSTEXPR ReturnT make_custom_exception(Ts&&... message)
{
	std::stringstream ss;
	(ss << ... << std::forward<Ts>(message));
	return{ ss.str() };
}

/**
 * @brief			Create an exception with a message.
 * @param message	Any number of types with a std::ostream& operator<<
 * @returns			except
 */
template<var::Streamable<std::stringstream>... Ts>
WINCONSTEXPR except make_exception(Ts&&... message)
{
	return make_custom_exception<except>(std::forward<Ts>(message)...);
}

/**
 * @brief			Create an exception with a given message. 
 *\n				This function only accepts wide-char-based types, and truncates all 
 *\n				of the characters to 1 byte as exception messages cannot contain w
 * @param message	Any number of types with a std::wostream& operator<<
 * @returns			except
 */
template<std::derived_from<std::exception> ReturnT, var::Streamable<std::wstringstream>... Ts>
WINCONSTEXPR except make_exeption(Ts&&... message)
{
	std::wstring wstr;
	{
		std::wstringstream ss;
		(ss << ... << std::forward<Ts>(message));
		wstr = ss.str();
	}
	std::string str;
	for (auto& wch : wstr)
		str += static_cast<char>(wch);
	return { str.c_str() };
}

