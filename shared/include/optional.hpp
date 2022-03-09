/**
 * @file	optional.hpp
 * @author	radj307
 * @brief	Contains a stack-based alternative to the std::optional type.
 */
#pragma once
#include <sysarch.h>
#include <make_exception.hpp>
#include <var.hpp>

#include <optional>

 /**
  * @namespace	optional
  * @brief		Contains a stack-based alternative to the std::optional type, with some useful derived types.
  */
namespace optional {
	/**
	 * @brief		Adds a third variable state to any type: "null". This is an alternative to the std::optional object that uses the stack.
	 *\n			Note: This function does not allocate memory on the heap, and should only be used for small variable types as a wrapper.
	 * @tparam T:	Value Type.
	*/
	template<typename T>
	struct optional {
	protected:
		T _value;
		bool _has_value;

	public:
		template<typename... Args>
		explicit optional(Args&&... init_args) : _value{ std::forward<Args>(init_args)... }, _has_value{ true } {}

		/**
		 * @brief			Value-move constructor.
		 * @param value:	rvalue reference of a value to initialize the optional with.
		 */
		optional(T&& value) : _value{ std::move(value) }, _has_value{ true } {}
		/**
		 * @brief			Value-copy constructor.
		 * @param value:	const lvalue reference of a value to initialize the optional with.
		 */
		optional(const T& value) : _value{ value }, _has_value{ true } {}

		optional(optional<T>&& o) : _value{ std::move(o._value) }, _has_value{ std::move(o._has_value) } {}
		optional(const optional<T>& o) : _value{ o._value }, _has_value{ o._has_value } {}

		optional(const std::nullopt_t& nullopt = std::nullopt) : _has_value{ false } {}

		[[nodiscard]] bool has_value() const noexcept
		{
			return _has_value;
		}
		[[nodiscard]] T value() const noexcept(false)
		{
			if (_has_value)
				return _value;
			throw make_exception("optional::value():  Null Value Exception!");
		}
		[[nodiscard]] T value_or(auto&& alt) const noexcept
		{
			return _has_value
				? _value
				: std::forward<decltype(alt)>(alt);
		}

		optional<T>& operator=(T&& value) noexcept
		{
			_value = std::move(value);
			_has_value = true;
			return *this;
		}
		optional<T>& operator=(const T& value) noexcept
		{
			_value = std::move(value);
			_has_value = true;
			return *this;
		}
		optional<T>& operator=(const std::nullopt_t& nullopt) noexcept
		{
			_has_value = false;
			return *this;
		}
	};

	template<typename T>
	struct optional_passthru : optional<T> {
		T default_value;

		void setDefaultValue(T&& value)
		{
			default_value = std::forward<T>(value);
		}
		T getDefaultValue() const
		{
			return default_value;
		}

		optional_passthru(T&& value, T&& defaultValue) : optional<T>(std::forward<T>(value)), default_value{ std::forward<T>(defaultValue) } {}
		optional_passthru(const T& value, const T& defaultValue) : optional<T>(value), default_value{ defaultValue } {}

		optional_passthru(const std::nullopt_t& nullopt, T&& defaultValue) : optional<T>(nullopt), default_value{ std::forward<T>(defaultValue) } {}
		optional_passthru(const std::nullopt_t& nullopt, const T& defaultValue) : optional<T>(nullopt), default_value{ defaultValue } {}

		optional_passthru(const std::nullopt_t& nullopt = std::nullopt) : optional<T>(nullopt) {}

		operator T() const
		{
			if (_has_value)
				return _value;
			return default_value;
		}
	};
}
