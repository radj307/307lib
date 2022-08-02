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
	 * @class	basic_except
	 * @brief	General-purpose std::exception wrapper object that adds useful features like constructors and mutable error messages.
	 *\n		This exception type can be caught by any `catch` statement for std::exception.
	 */
	class basic_except : public std::exception {
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
		basic_except() = default;
		/**
		 * @brief			Constructor with a message.
		 * @param message	A string to print when calling the what() method.
		 */
		basic_except(std::string&& message) : message{ std::move(std::make_unique<std::string>(std::move(message))) } {}
		/**
		 * @brief			Constructor with a message.
		 * @param message	A string to print when calling the what() method.
		 */
		basic_except(const std::string& message) : message{ std::move(std::make_unique<std::string>(message)) } {}
		/**
		 * @brief			Move Constructor
		 * @param o			Another except object to move.
		 */
		basic_except(basic_except&& o) noexcept : message{ std::move(o.message) } {}

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
		friend std::ostream& operator<<(std::ostream& os, const basic_except& ex) { return os << ex.what(); }
	};

	using except = basic_except;

	/**
	 * @class	except_with_id
	 * @brief	Extends the basic_except class with exception identifier numbers, commonly known as 'error codes'.
	 */
	class except_with_id : public basic_except {
		using base = basic_except;
		long long errorID{ 0 };

	public:
		/**
		 * @brief			Constructor
		 * @param message	A string to print when calling the what() method.
		 * @param errorID	An error code to associate with this exception.
		 */
		except_with_id(std::string&& message, const long long& errorID) : base(std::move(message)), errorID{ errorID } {}
		/**
		 * @brief			Constructor
		 * @param message	A string to print when calling the what() method.
		 * @param errorID	An error code to associate with this exception.
		 */
		except_with_id(const std::string& message, const long long& errorID) : base(message), errorID{ errorID } {}
		using base::base; //< inherit base constructors

		/**
		 * @brief		Retrieve the error code associated with this exception.
		 * @returns		The error code associated with this exception.
		 */
		[[nodiscard]] virtual constexpr long long error_code() const noexcept { return this->operator long long(); }

		/**
		 * @brief		Allows implicit conversion to the error code type.
		 * @returns		The error code associated with this exception.
		 */
		[[nodiscard]] constexpr operator long long() const noexcept { return errorID; }
	};

	/**
	 * @brief				Create a custom exception type with the given arguments as a message.
	 * @tparam ReturnT		The type of exception to return.
	 * @tparam ...Ts		Variadic Templated Types.
	 * @param ...message	The message shown when calling the what() function.
	 * @returns				ReturnT
	 */
	template<std::derived_from<except> ReturnT, var::streamable<std::ostream>... Ts> requires std::constructible_from<ReturnT, std::string>
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
	template<std::derived_from<except> ReturnT, var::streamable<std::ostream>... Ts> requires var::more_than<1ull, Ts...>&& std::constructible_from<ReturnT, Ts...>
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
	template<var::streamable<std::ostream>... Ts>
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
	template<std::derived_from<except> ReturnT, var::streamable<std::wostream>... Ts>
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

#ifndef MAKE_EXCEPTION_NOGLOBAL
using ex::make_custom_exception;
using ex::make_exception;
#endif

#ifndef MAKE_EXCEPTION_SIMPLE
/**
 * @def		MAKE_EXCEPTION_HPP_SIMPLE
 * @brief	When defined, this macro will disable <make_exception.hpp>'s advanced feature set,
 *\n		including derivatives of the except object & the DEFINE_EXCEPTION & GET_EXCEPTION macros.
 *
 * @todo	This section is incomplete.
 */
#define MAKE_EXCEPTION_SIMPLE
#undef MAKE_EXCEPTION_SIMPLE

 /**
  * @def			$DEFINE_EXCEPT
  * @brief			Creates a new exception object derived from ex::except that can be used in a multi-catch statement (see try-catch) to handle different exceptions created with make_custom_exception.
  * @param name		The name to use for the exception object, which will be appended to "except_" as the name of a struct definition.
  *\n				DO NOT PASS A QUOTE-ENCLOSED STRING HERE!
  */
#define $DEFINE_EXCEPT(name) struct except_##name : ::ex::except { using base = ::ex::except; using base::base; };
  /**
   * @def			$EXCEPT
   * @brief			Gets the name of an exception object that was previously-defined with DEFINE_EXCEPTION.
   * @param name	The name of a previously-created exception object.
   *\n				DO NOT PASS A QUOTE-ENCLOSED STRING HERE!
   */
#define $EXCEPT(name) except_##name

#endif

#ifndef MAKE_EXCEPTION_SIMPLE
 // TODO: Make specific exception type overrides for the set of std::exception derivatives available in the stdlib.
#endif
