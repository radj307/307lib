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
protected:
	/// @brief	The message associated with this exception
	std::string message;

public:
	/**
	 * @brief	Default constructor with a blank message.
	 */
	except() = default;
	/**
	 * @brief			Constructor with a message.
	 * @param message	A string to print when calling the what() method.
	 */
	except(auto&& message) : message{ std::forward<decltype(message)>(message) } {}
	/**
	 * @brief	Retrieve the message associated with this exception.
	 * @returns	const char*
	 */
	virtual const char* what() const noexcept override { return message.c_str(); }
	/**
	 * @brief		Insert the message associated with this exception into an output stream.
	 *\n			This is the same as calling the what() method.
	 * @param os	Target Output Stream Instance
	 * @param ex	Constant Reference of an except object.
	 * @returns		std::ostream&
	 */
	friend std::ostream& operator<<(std::ostream& os, const except& ex) { return os << ex.what(); }
};

/**
 * @brief				Create a custom exception type with the given arguments as a message.
 * @tparam ReturnT		The type of exception to return.
 * @tparam ...Ts		Variadic Templated Types.
 * @param ...message	The message shown when calling the what() function.
 * @returns				ReturnT
 */
template<std::derived_from<except> ReturnT, var::Streamable<std::stringstream>... Ts>
WINCONSTEXPR ReturnT make_custom_exception(Ts&&... message)
{
	std::stringstream ss;
	(ss << ... << std::forward<Ts>(message));
	return ReturnT{ ss.str() };
}
/**
 * @brief				Create a custom exception type with the given arguments as a message.
 * @tparam ReturnT		The type of exception to return.
 * @tparam ...VT		Variadic Templated Types.
 * @param ...message	The message shown when calling the what() function.
 * @returns				ReturnT
 */
template<std::derived_from<except> ReturnT, typename... Arguments>
WINCONSTEXPR ReturnT make_custom_exception(const std::string& message, Arguments&&... args)
{
	return ReturnT{ message, std::forward<Arguments>(args)... };
}

/**
 * @brief			Create an exception with a given message.
 * @tparam ...Ts	Any number of types with an overloaded operator<< for std::ostream.
 * @param message	Any number of printable variables/objects.
 * @returns			except
 */
template<var::Streamable<std::stringstream>... Ts>
WINCONSTEXPR except make_exception(Ts&&... message)
{
	return make_custom_exception<except>(std::forward<Ts>(message)...);
}

/**
 * @brief			Create an exception with a given message.
 *\n				This function only accepts types using wchar_t instead of char, however all characters are 
 *\n				converted to 1 byte chars before returning, as exception messages cannot contain wide chars.
 * @tparam ...Ts	Any number of types with an overloaded operator<< for std::wostream.
 * @param message	Any number of printable variables/objects.
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
