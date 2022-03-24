#pragma once
#include <var.hpp>
#include <indentor.hpp>

#include <memory>
#include <string>
#include <sstream>
#include <exception>

/**
 * @namespace	ex
 * @brief		Contains objects & functions used to conveniently create platform-agnostic derivatives of the std::exception object.
 */
namespace ex {
	/**
	 * @class		except  make_exception.hpp
	 * @extends		std::exception
	 * @exception	except	Generic exception type derived directly from std::exception that provides a convenient wrapper interface this is used by the make_exception() functions, and may be derived from to further expand on the available feature set.
	 * @brief		Exception object with a custom message stored as a member variable.
	 *\n
	 *\n			A std::string pointer is used to bypass the constness of std::exception::what()
	 *\n			 in order to allow modification of the message by derived objects that implement
	 *\n			 an override for the virtual what() method.
	 */
	class except : public std::exception {
		/**
		 * @private
		 * @var
		 * @brief	The message associated with this exception
		 */
		std::unique_ptr<std::string> message{ std::make_unique<std::string>() };

	protected:
		/**
		 * @protected
		 * @brief		Retrieve a pointer to this except object's message string.
		 * @returns		std::string*
		 */
		[[nodiscard]] virtual std::string* get_message() const { return message.get(); }

		/**
		 * @protected
		 * @brief		This function is called by the default ex::except::what()
		 *\n			 function before returning the final exception message.
		 *\n			This method is provided as a safer alternative to directly
		 *\n			 implementing overrides for the what() function in derived objects.
		 */
		virtual void format() const noexcept {}

	public:
		/**
		 * @brief	Default constructor with a blank message.
		 */
		except() = default;
		/**
		 * @brief			Constructor with a message.
		 * @param message	A string to print when calling the what() method.
		 */
		except(std::string&& message) : message{ std::move(std::make_unique<std::string>(std::move(message))) } {}
		/**
		 * @brief			Constructor with a message.
		 * @param message	A string to print when calling the what() method.
		 */
		except(const std::string& message) : message{ std::move(std::make_unique<std::string>(message)) } {}
		/**
		 * @brief			Move Constructor
		 * @param o			Another except object to move.
		 */
		except(except&& o) : message{ std::move(o.message) } {}

		/**
		 * @brief		Retrieve the message associated with this exception.
		 * @returns		const char*
		 *
		 * @deprecated	Implementing an override for this function is deprecated! Use format() in derived objects instead.
		 */
		[[nodiscard]] virtual const char* what() const noexcept override
		{
			format();
			return message.get()->c_str();
		}

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
	template<std::derived_from<except> ReturnT, var::Streamable<std::ostream>... Ts> requires std::constructible_from<ReturnT, std::string>
	[[nodiscard]] WINCONSTEXPR ReturnT make_custom_exception(Ts&&... message)
	{
		std::stringstream ss;
		(ss << ... << std::forward<Ts>(message));
		return ReturnT{ ss.str() };
	}

	/**
	 * @brief				Create a custom exception type with the given arguments as a message.
	 * @tparam ReturnT		The type of exception to return.
	 * @tparam ...Ts		Variadic Templated Types.
	 * @param ...message	The message shown when calling the what() function.
	 * @returns				ReturnT
	 */
	template<std::derived_from<except> ReturnT, var::Streamable<std::ostream>... Ts> requires var::more_than<1ull, Ts...> && std::constructible_from<ReturnT, Ts...>
	[[nodiscard]] WINCONSTEXPR ReturnT make_custom_exception(Ts&&... message)
	{
		return ReturnT{ std::forward<Ts>(message)... };
	}

	/**
	 * @brief			Create an exception with a given message.
	 * @tparam ...Ts	Any number of types with an overloaded operator<< for std::ostream.
	 * @param message	Any number of printable variables/objects.
	 * @returns			except
	 */
	template<var::Streamable<std::ostream>... Ts>
	[[nodiscard]] WINCONSTEXPR except make_exception(Ts&&... message)
	{
		return ex::make_custom_exception<ex::except>(std::forward<Ts>(message)...);
	}

	/**
	 * @overload
	 * @brief			Create an exception with a given message, using wide-characters.
	 *\n				This function only accepts types using wchar_t instead of char, however all characters are
	 *\n				 converted to narrow chars before returning, as exception messages cannot contain wide chars.
	 * @tparam ...Ts	Any number of types with an overloaded operator<< for std::wostream.
	 * @param message	Any number of printable variables/objects.
	 * @returns			except
	 */
	template<std::derived_from<except> ReturnT, var::Streamable<std::wostream>... Ts>
	[[nodiscard]] WINCONSTEXPR except make_exeption(Ts&&... message)
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
}

#ifndef MAKE_EXCEPTION_HPP_NOGLOBAL
using ex::make_custom_exception;
using ex::make_exception;
#endif

#ifndef MAKE_EXCEPTION_HPP_SIMPLE
/**
 * @def		MAKE_EXCEPTION_HPP_SIMPLE
 * @brief	When defined, this macro will disable <make_exception.hpp>'s advanced feature set,
 *\n		including derivatives of the except object.
 *
 * @todo	This section is incomplete.
 */
#define MAKE_EXCEPTION_HPP_SIMPLE
#undef MAKE_EXCEPTION_HPP_SIMPLE
#endif

#ifndef MAKE_EXCEPTION_HPP_SIMPLE
 // TODO: Make specific exception type overrides for the set of std::exception derivatives available in the stdlib.
#endif
