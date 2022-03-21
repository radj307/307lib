/**
 * @file	optional.hpp
 * @author	radj307
 * @brief	Contains useful derivatives of the std::optional object, such as casting_optional.
 *\n		These are focused on adding functionality to the std::optional object, and can be used as a drop-in replacement.
 */
#pragma once
#include <sysarch.h>
#include <make_exception.hpp>
#include <var.hpp>

#include <optional>

/**
 * @namespace	opt
 * @brief		Namespace containing functions & objects related to "optional" variables.
 */
namespace opt {
	/**
	 * @struct		casting_optional
	 * @brief		Wrapper for std::optional that includes several helpful typecasting functions.
	 *\n			This inherits & forwards most of its functionality from std::optional.
	 *\n			See functions with the word "cast" in their name for a list of new functions.
	 * @tparam T	Value Type.
	 */
	template<typename T>
	struct casting_optional : public std::optional<T> {
		/// @brief	Contained value's type.
		using type = T;
		/// @brief	Regular optional type from which this wrapper is derived.
		using base = std::optional<type>;

		/// @brief	Inherited Constructor(s).
		using base::base;
		/// @brief	Inherited Assignment Operator(s).
		using base::operator=;

		/**
		 * @brief		Decay this optional wrapper into a regular optional type.
		 * @returns		std::optional<T>&
		 */
		operator base& () { return *this; }
		/**
		 * @brief		Decay this optional wrapper into a regular optional type.
		 * @returns		std::optional<T>
		 */
		operator base() const { return *this; }

		/**
		 * @brief								Retrieve the value, casted to another type using static_cast.
		 * @tparam U							Another type that can be trivially converted to from type T.
		 * @returns								U
		 * @throws std::bad_optional_access		The underlying value isn't set, and cannot be retrieved.
		 */
		template<var::convertible_from<T> U>
		[[nodiscard]] constexpr U cast_value() const noexcept(false)
		{
			if (!this->_Has_value)
				throw std::bad_optional_access{};
			return static_cast<U>(this->_Value);
		}

		/**
		 * @brief								Retrieve the value, casted to another type using a provided conversion function.
		 * @tparam U							Any other type.
		 * @param converter						A converter function that accepts type T and returns type U.
		 * @returns								U
		 * @throws std::bad_optional_access		The underlying value isn't set, and cannot be retrieved.
		 */
		template<typename U>
		[[nodiscard]] constexpr U cast_value(const std::function<U(T)>& converter) const noexcept(false)
		{
			if (!this->_Has_value)
				throw std::bad_optional_access{};
			return converter(this->_Value);
		}

		/**
		 * @brief					Retrieve the value, casted to another type using static_cast.
		 * @tparam U				Another type that can be trivially converted to from type T.
		 * @param default_value		An alternative value to return if has_value() is false.
		 * @returns					U
		 */
		template<var::convertible_from<T> U>
		[[nodiscard]] constexpr U cast_value_or(const U& default_value) const noexcept
		{
			if (!this->_Has_value)
				return default_value;
			return static_cast<U>(this->_Value);
		}

		/**
		 * @brief					Retrieve the value, casted to another type using a provided conversion function.
		 * @tparam U				Any other type.
		 * @param default_value		An alternative value to return if has_value() is false.
		 * @param converter			A converter function that accepts type T and returns type U.
		 * @returns					U
		 */
		template<typename U>
		[[nodiscard]] constexpr U cast_value_or(const U& default_value, const std::function<U(T)>& converter) const noexcept
		{
			if (!this->_Has_value)
				return default_value;
			return converter(this->_Value);
		}

		/**
		 * @brief				Retrieve the value, casted to another type using static_cast.
		 * @tparam U			Another type that can be trivially converted to from type T
		 * @param alternate		An alternative value to return if has_value() is false.
		 * @returns				U
		 */
		template<var::convertible_from<T> U>
		[[nodiscard]] constexpr U cast_value_or(const T& alternate) const noexcept
		{
			if (!this->_Has_value)
				return static_cast<U>(alternate);
			return static_cast<U>(this->_Value);
		}

		/**
		 * @brief				Retrieve the value, casted to another type using a provided conversion function.
		 * @tparam U			Any other type.
		 * @param alternate		An alternative value to pass to the converter if has_value() is false.
		 * @param converter		A converter function that accepts type T and returns type U.
		 * @returns				U
		 */
		template<typename U>
		[[nodiscard]] constexpr U cast_value_or(const T& alternate, const std::function<U(T)>& converter) const noexcept
		{
			if (!this->_Has_value)
				return converter(alternate);
			return converter(this->_Value);
		}

		/**
		 * @brief		Copy this optional object and cast its value to another type.
		 * @tparam U	Another type that can be converted to from type T using static_cast.
		 * @returns		casting_optional<U>
		 */
		template<var::convertible_from<T> U>
		[[nodiscard]] constexpr casting_optional<U> cast() const noexcept
		{
			if (this->_Has_value)
				return casting_optional<U>{ static_cast<U>(this->_Value) };
			return casting_optional<U>{ std::nullopt };
		}

		/**
		 * @brief				Copy this optional object and cast its value to another type.
		 * @tparam U			Any other type.
		 * @param converter		A converter function that can convert from type T to type U.
		 * @returns				casting_optional<U>
		 */
		template<typename U>
		[[nodiscard]] constexpr casting_optional<U> cast(const std::function<U(T)>& converter) const noexcept
		{
			if (this->_Has_value)
				return casting_optional<U>{ converter(this->_Value) };
			return casting_optional<U>{ std::nullopt };
		}
	};

	/**
	 * @brief		Alias for the casting_optional object.
	 * @tparam T	Value Type
	 */
	template<typename T> using optional = casting_optional<T>;

	/**
	 * @struct			unsafe_optional
	 * @brief			An "unsafe" wrapper object that allows implicit "casting"/retrieval of the underlying value.
	 *\n				If the value isn't set, an exception will be thrown during an implicit casting operation, making debugging very difficult.
	 *\n				For this reason, it is NOT recommended to use the unsafe_optional object in production code.
	 * @tparam T		Value Type
	 * @tparam Base		Base optional type. This defaults to casting_optional, but any type derived from std::optional is accepted.
	 */
	template<typename T, std::derived_from<std::optional<T>> Base = casting_optional<T>>
	struct unsafe_optional : public Base {
		/// @brief	Contained value's type.
		using type = T;
		/// @brief	Regular optional type from which this wrapper is derived.
		using base = std::optional<type>;

		/// @brief	Inherited Constructor(s).
		using base::base;
		/// @brief	Inherited Assignment Operator(s).
		using base::operator=;

		/**
		 * @brief		Decay this optional wrapper into a regular optional type.
		 * @returns		std::optional<T>&
		 */
		operator base& () { return *this; }
		/**
		 * @brief		Decay this optional wrapper into a regular optional type.
		 * @returns		std::optional<T>
		 */
		operator base() const { return *this; }

		/**
		 * @brief	Casting operator that retrieves the underlying value, if it is set.
		 *\n		If there is no value set, an exception will be thrown during the casting operation!
		 * @returns	T&
		 * @throws	std::bad_optional_access
		 */
		operator T& () noexcept(false)
		{
			if (!this->_Has_value)
				throw std::bad_optional_access{};
			return this->_Value;
		}
		/**
		 * @brief	Casting operator that retrieves the underlying value, if it is set.
		 *\n		If there is no value set, an exception will be thrown during the casting operation!
		 * @returns	T
		 * @throws	std::bad_optional_access
		 */
		operator T() const noexcept(false)
		{
			if (!this->_Has_value)
				throw std::bad_optional_access{};
			return this->_Value;
		}
	};
}
