#pragma once
/**
 * @file	make_exception.hpp
 * @author	radj307
 * @brief	Provides system-agnostic exception objects with const-time formatting capabilities, as well as preprocessor definitions to quickly define new exception objects.
 */
#include <sysarch.h>
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
	 * @class	except
	 * @brief	General-purpose std::exception wrapper object that adds useful features like constructors and mutable error messages.
	 *\n		This exception type can be caught by any `catch` statement for std::exception.
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
		except(except&& o) noexcept : message{ std::move(o.message) } {}

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
	 * @tparam TReturn		The type of exception to return.
	 * @tparam ...Ts		Variadic Templated Types.
	 * @param ...message	The message shown when calling the what() function.
	 * @returns				TReturn
	 */
	template<std::derived_from<except> TReturn, var::streamable<std::ostream>... Ts> requires std::constructible_from<TReturn, std::string>
	[[nodiscard]] WINCONSTEXPR TReturn make_custom_exception(Ts&&... message)
	{
		std::stringstream ss;
		(ss << ... << std::forward<Ts>(message));
		return TReturn{ ss.str() };
	}

	/**
	 * @brief				Create a custom exception type from the given arguments.
	 * @details				This overload differs from `make_custom_exception` in that it passes the arguments ***directly*** to the TReturn type's constructor.
	 *\n					Because of this, it is required that `TReturn` is constructible from the types specified in the order that they are specified.
	 * @tparam TReturn		The type of exception to return.
	 * @tparam ...Ts		Variadic Templated Types.
	 * @param ...args		Arguments to pass to the constructor of `TReturn`.
	 * @returns				A new `except`-derived instance of type `TReturn`, created using the constructor with the signature `Ts...`.
	 *\n					If no such constructor exists, compilation fails.
	 */
	template<std::derived_from<except> TReturn, var::streamable<std::ostream>... Ts> requires std::constructible_from<TReturn, Ts...>
	[[nodiscard]] WINCONSTEXPR TReturn make_custom_exception_explicit(Ts&&... args)
	{
		return TReturn{ std::forward<Ts>(args)... };
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
		return make_custom_exception<ex::except>(std::forward<Ts>(message)...);
	}
}

#ifndef MAKE_EXCEPTION_NOGLOBAL
using ex::make_custom_exception_explicit;
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
  * @def		$DefineExcept
  * @brief		Creates a struct definition with the given typename and optionally, contents.
  * @param name	The name to give to the exception instance.
  * @param ...	Any number of lines of code to insert into the new exception instance.
  *				You can use this to define constructors, methods, members, etc; the following code is always implicit:
  *				```cpp
  *				using this_t = $_$unwrap(name);
  *				using base_t = ::ex::except;
  *				using base_t::base_t;
  *				```
  *				Both `this_t` & `base_t` may be used by your code in this section. Note that the base `ex::except` constructors are also implicitly pulled !
  */
#define $DefineExcept(name, ...) struct $_$unwrap(name) : public ::ex::except { using this_t = $_$unwrap(name); using base_t = ::ex::except; using base_t::base_t; __VA_ARGS__ };

#endif

#ifndef MAKE_EXCEPTION_SIMPLE
  // TODO: Make specific exception type overrides for the set of std::exception derivatives available in the stdlib.
#endif
