/**
 * @file	color-transform.hpp
 * @author	radj307
 * @brief	Contains functions for transforming color values between different formats.
 */
#pragma once
#include <sysarch.h>
#include <make_exception.hpp>
#include <var.hpp>
#include <math.hpp>

#include <cmath>
#include <utility>
#include <sstream>
#include <iomanip>

namespace color {
	/**
	 * @struct			basic_color
	 * @brief			Variable-channel color wrapper object that inherits from std::tuple.
	 * @tparam T		Any integral type. All variadic arguments must be the same type as this.
	 * @tparam Ts...	Any number of additional channels.
	 */
	template<std::integral T, std::integral... Ts> requires var::all_same<T, Ts...>
	struct basic_color : public std::tuple<T, Ts...> {
		using base = std::tuple<T, Ts...>;
		/// @brief	The value type used by each channel.
		using value = T;

		constexpr basic_color() : base(std::make_index_sequence<1ull + sizeof...(Ts)>()) {}
		constexpr basic_color(const T& fst, const Ts&... rest) : base(fst, rest...) {}
		constexpr basic_color(T&& fst, Ts&&... rest) : base(std::forward<T>(fst), std::forward<Ts>(rest)...) {}

		/**
		 * @brief	Retrieve the number of independent color channels, which is always equal to the number of template arguments.
		 * @returns	size_t with the number of independent color channels.
		 */
		constexpr size_t channel_count() const { return 1ull + sizeof...(Ts); }

		/**
		 * @brief			Get the current value of the specified color channel.
		 * @tparam Index	The index in the underlying tuple type to retrieve.
		 * @returns			value with the current value of the channel with the specified Index.
		 */
		template<size_t Index>
		[[nodiscard]] constexpr value get() const
		{
			static_assert(Index < 1ull + sizeof...(Ts), "Index is out-of-range for a color with this number of channels.");
			return std::get<Index>(*this);
		}
		/**
		 * @brief			Get the current value of the specified color channel. This overload returns a mutable reference.
		 * @tparam Index	The index in the underlying tuple type to retrieve.
		 * @returns			value with the current value of the channel with the specified Index.
		 */
		template<size_t Index>
		[[nodiscard]] constexpr value& get()
		{
			static_assert(Index < 1ull + sizeof...(Ts), "Index is out-of-range for a color with this number of channels.");
			return std::get<Index>(*this);
		}

		/**
		 * @brief			Static function that applies a given lambda to each channel in 2 colors, and returns the result.
		 * @tparam Index	This is used internally for template recursion.
		 * @param l			Left-side operand, of type `basic_color<T, Ts...>&`.
		 * @param r			Right-side operand, of type `basic_color<T, Ts...> const&`.
		 * @param oper		An operator lambda.
		 * @returns			`basic_color<T, Ts...>&` with the result.
		 */
		template<size_t Index = 0ull>
		friend constexpr basic_color<T, Ts...>& transform(basic_color<T, Ts...>& l, basic_color<T, Ts...> const& r, const std::function<value(value, value)>& oper)
		{
			static_assert(Index < 1ull + sizeof...(Ts), "Index is out-of-range for a color with this number of channels.");
			l.get<Index>() = oper(l.get<Index>(), r.get<Index>());
			if constexpr (Index + 1ull < 1ull + sizeof...(Ts)) return transform<Index + 1ull>(l, r, oper);
			return l;
		}
		/**
		 * @brief			Static function that applies a given lambda to each channel of a color and a single value of any integral type, and returns the result.
		 * @tparam U		Any integral type.
		 * @tparam Index	This is used internally for template recursion.
		 * @param l			Left-side operand, of type `basic_color<T, Ts...>&`
		 * @param r			Right-side operand, of type `U`.
		 * @param oper		An operator lambda.
		 * @returns			`basic_color<T, Ts...>&` with the result.
		 */
		template<std::integral U, size_t Index = 0ull>
		friend constexpr basic_color<T, Ts...>& transform(basic_color<T, Ts...>& l, U const& r, const std::function<value(value, value)>& oper)
		{
			static_assert(Index < 1ull + sizeof...(Ts), "Index is out-of-range for a color with this number of channels.");
			l.get<Index>() = oper(l.get<Index>(), r);
			if constexpr (Index + 1ull < 1ull + sizeof...(Ts)) return transform<Index + 1ull>(l, r, oper);
			return l;
		}

		// addition
		/**
		 * @brief	Addition Operator
		 * @param o	Another color instance with the same number of channels.
		 * @returns basic_color<T, Ts...>
		 */
		constexpr basic_color<T, Ts...> operator+(basic_color<T, Ts...>&& o) const
		{
			basic_color<T, Ts...> out{ *this };
			return transform(out, std::forward<basic_color<T, Ts...>>(o), [](value&& l, value&& r) -> value { return std::forward<value>(l) + std::forward<value>(r); });
		}
		constexpr basic_color<T, Ts...>& operator+=(basic_color<T, Ts...>&& o)
		{
			return *this = transform(*this, std::forward<basic_color<T, Ts...>>(o), [](value&& l, value&& r) -> value { return std::forward<value>(l) + std::forward<value>(r); });
		}
		template<std::integral U>
		constexpr basic_color<T, Ts...> operator+(U&& o) const
		{
			basic_color<T, Ts...> out{ *this };
			return transform(out, std::forward<U>(o), [](value&& l, value&& r) -> value { return std::forward<value>(l) + std::forward<value>(r); });
		}
		template<std::integral U>
		constexpr basic_color<T, Ts...>& operator+=(U&& o)
		{
			return *this = transform(*this, std::forward<U>(o), [](value&& l, value&& r) -> value { return std::forward<value>(l) + std::forward<value>(r); });
		}
		// subtraction
		constexpr basic_color<T, Ts...> operator-(basic_color<T, Ts...>&& o) const
		{
			basic_color<T, Ts...> out{ *this };
			return transform(out, std::forward<basic_color<T, Ts...>>(o), [](value&& l, value&& r) -> value { return std::forward<value>(l) - std::forward<value>(r); });
		}
		constexpr basic_color<T, Ts...>& operator-=(basic_color<T, Ts...>&& o)
		{
			return *this = transform(*this, std::forward<basic_color<T, Ts...>>(o), [](value&& l, value&& r) -> value { return std::forward<value>(l) - std::forward<value>(r); });
		}
		template<std::integral U>
		constexpr basic_color<T, Ts...> operator-(U&& o) const
		{
			basic_color<T, Ts...> out{ *this };
			return transform(out, std::forward<U>(o), [](value&& l, value&& r) -> value { return std::forward<value>(l) - std::forward<value>(r); });
		}
		template<std::integral U>
		constexpr basic_color<T, Ts...>& operator-=(U&& o)
		{
			return *this = transform(*this, std::forward<U>(o), [](value&& l, value&& r) -> value { return std::forward<value>(l) - std::forward<value>(r); });
		}
		// multiplication
		constexpr basic_color<T, Ts...> operator*(basic_color<T, Ts...>&& o) const
		{
			basic_color<T, Ts...> out{ *this };
			return transform(out, std::forward<basic_color<T, Ts...>>(o), [](value&& l, value&& r) -> value { return std::forward<value>(l) * std::forward<value>(r); });
		}
		constexpr basic_color<T, Ts...>& operator*=(basic_color<T, Ts...>&& o)
		{
			return *this = transform(*this, std::forward<basic_color<T, Ts...>>(o), [](value&& l, value&& r) -> value { return std::forward<value>(l) * std::forward<value>(r); });
		}
		template<std::integral U>
		constexpr basic_color<T, Ts...> operator*(U&& o) const
		{
			basic_color<T, Ts...> out{ *this };
			return transform(out, std::forward<U>(o), [](value&& l, value&& r) -> value { return std::forward<value>(l) * std::forward<value>(r); });
		}
		template<std::integral U>
		constexpr basic_color<T, Ts...>& operator*=(U&& o)
		{
			return *this = transform(*this, std::forward<U>(o), [](value&& l, value&& r) -> value { return std::forward<value>(l) * std::forward<value>(r); });
		}
		// division
		constexpr basic_color<T, Ts...> operator/(basic_color<T, Ts...>&& o) const
		{
			basic_color<T, Ts...> out{ *this };
			return transform(out, std::forward<basic_color<T, Ts...>>(o), [](value&& l, value&& r) -> value { return std::forward<value>(l) / std::forward<value>(r); });
		}
		constexpr basic_color<T, Ts...>& operator/=(basic_color<T, Ts...>&& o)
		{
			return *this = transform(*this, std::forward<basic_color<T, Ts...>>(o), [](value&& l, value&& r) -> value { return std::forward<value>(l) / std::forward<value>(r); });
		}
		template<std::integral U>
		constexpr basic_color<T, Ts...> operator/(U&& o) const
		{
			basic_color<T, Ts...> out{ *this };
			return transform(out, std::forward<U>(o), [](value&& l, value&& r) -> value { return std::forward<value>(l) / std::forward<value>(r); });
		}
		template<std::integral U>
		constexpr basic_color<T, Ts...>& operator/=(U&& o)
		{
			return *this = transform(*this, std::forward<U>(o), [](value&& l, value&& r) -> value { return std::forward<value>(l) / std::forward<value>(r); });
		}
	};

	/// @brief	Default type used to store color channel values.
	using ColorT = unsigned char;

	/**
	 * @struct		RGB
	 * @brief		A 3-channel RGB color type.
	 * @details		Channels:  Red, Green, Blue
	 * @tparam T	Any integral type. This is used as the value type for each color channel.
	 */
	template<std::integral T = ColorT>
	struct RGB : public basic_color<T, T, T> {
		/// @brief	Base/Parent object.
		using base = basic_color<T, T, T>;
		using base::base;
		using value = base::value;

		/// @brief	Red channel value.
		constexpr value r() const { return this->template get<0>(); }
		/// @brief	Green channel value.
		constexpr value g() const { return this->template get<1>(); }
		/// @brief	Blue channel value.
		constexpr value b() const { return this->template get<2>(); }

		/// @brief	Red channel reference.
		constexpr value& r() { return this->template get<0>(); }
		/// @brief	Green channel reference.
		constexpr value& g() { return this->template get<1>(); }
		/// @brief	Blue channel reference.
		constexpr value& b() { return this->template get<2>(); }
	};

	/**
	 * @struct		RGBA
	 * @brief		A 4-channel RGBA color type capable of storing RGB colors with transparency.
	 * @details		Channels:  Red, Green, Blue, Alpha (Transparency)
	 * @tparam T	Any integral type. This is used as the value type for each color channel.
	 */
	template<std::integral T = ColorT>
	struct RGBA : public basic_color<T, T, T, T> {
		/// @brief	Base/Parent object.
		using base = basic_color<T, T, T, T>;
		using base::base;
		using value = base::value;

		/// @brief	Red channel value.
		constexpr value r() const { return this->template get<0>(); }
		/// @brief	Green channel value.
		constexpr value g() const { return this->template get<1>(); }
		/// @brief	Blue channel value.
		constexpr value b() const { return this->template get<2>(); }
		/// @brief	Alpha channel value.
		constexpr value a() const { return this->template get<3>(); }

		/// @brief	Red channel reference.
		constexpr value& r() { return this->template get<0>(); }
		/// @brief	Green channel reference.
		constexpr value& g() { return this->template get<1>(); }
		/// @brief	Blue channel reference.
		constexpr value& b() { return this->template get<2>(); }
		/// @brief	Alpha channel reference.
		constexpr value& a() { return this->template get<3>(); }
		/**
		 * @brief	Explicit conversion operator for RGB values without an alpha channel.
		 * @returns	RGB
		 */
		constexpr explicit operator RGB<T>() const { return RGB{ r(), g(), b() }; }
	};

	/**
	 * @brief		Convert an RGB value to a SGR color value.
	 *\n			https://github.com/chadj2/bash-ui/blob/master/COLORS.md#xterm-colorspaces
	 * @tparam T	Any integral type.
	 * @param r		Red value. (Range: 0 - 5)
	 * @param g		Green value. (Range: 0 - 5)
	 * @param b		Blue value. (Range: 0 - 5)
	 * @returns		T
	 */
	template<std::integral T = ColorT>
	inline CONSTEXPR const T rgb_to_sgr(const T& r, const T& g, const T& b)
	{
		return static_cast<T>(static_cast<int>(r) * 36 + static_cast<int>(g) * 6 + (static_cast<int>(b) + 16));
	}

	/**
	 * @brief		Convert an RGB value to a SGR color value.
	 * @tparam T	Any floating-point type.
	 * @param r		Red value. (Range: 0.0 - 1.0)
	 * @param g		Green value. (Range: 0.0 - 1.0)
	 * @param b		Blue value. (Range: 0.0 - 1.0)
	 * @returns		ColorT
	 */
	template<std::floating_point T>
	inline
#ifndef OS_WIN
		CONSTEXPR
#endif
		ColorT rgb_to_sgr(const T& r, const T& g, const T& b) noexcept
	{
		T max{ var::largest<T>(r, g, b) };
		const auto translatef{ [&max](const auto& fp) -> ColorT {
			return static_cast<ColorT>(std::round(fp / max * static_cast<T>(5.0)));
		} };
		return rgb_to_sgr<ColorT>(translatef(r), translatef(g), translatef(b));
	}

	/**
	 * @brief				Convert an RGB value to an SGR value.
	 * @tparam T			Integral Type
	 * @param rgb_color		Input RGB color value.
	 * @returns				T
	 */
	template<std::integral T = ColorT>
	inline CONSTEXPR const T rgb_to_sgr(const RGB<T>& rgb_color) noexcept
	{
		return rgb_to_sgr<T>(rgb_color.template get<0>(), rgb_color.template get<1>(), rgb_color.template get<2>());
	}

	/**
	 * @brief				Convert an SGR color value into an RGB color value.
	 * @tparam T			Any integral type.
	 * @param sgr_color		An SGR color value.
	 * @returns				std::tuple<T, T, T>
	 */
	template<std::integral T = ColorT>
	inline CONSTEXPR const RGB<T> sgr_to_rgb(const T& sgr_color)
	{
		T tmp{ sgr_color - static_cast<T>(16) };

		T blue{ tmp % static_cast<T>(6) };
		tmp /= static_cast<T>(6);
		T green{ tmp % static_cast<T>(6) };
		tmp /= static_cast<T>(6);

		return{ tmp, green, blue };
	}

#pragma region Hexadecimal_Transformations

	/**
	 * @brief		Check if the given character is a valid hexadecimal value. (Range: 0 - 9 | a - f | A - F)
	 * @param ch	Input Character.
	 * @returns		bool
	 */
	inline static CONSTEXPR bool ishexnum(const char& ch)
	{
		switch (ch) {
		case '0': [[fallthrough]];
		case '1': [[fallthrough]];
		case '2': [[fallthrough]];
		case '3': [[fallthrough]];
		case '4': [[fallthrough]];
		case '5': [[fallthrough]];
		case '6': [[fallthrough]];
		case '7': [[fallthrough]];
		case '8': [[fallthrough]];
		case '9': [[fallthrough]];
		case 'a': [[fallthrough]];
		case 'A': [[fallthrough]];
		case 'b': [[fallthrough]];
		case 'B': [[fallthrough]];
		case 'c': [[fallthrough]];
		case 'C': [[fallthrough]];
		case 'd': [[fallthrough]];
		case 'D': [[fallthrough]];
		case 'e': [[fallthrough]];
		case 'E': [[fallthrough]];
		case 'f': [[fallthrough]];
		case 'F':
			return true;
		default:
			return false;
		}
	}

	/**
	 * @brief				Convert a single hexadecimal character to an integral value.
	 * @tparam ReturnType	Type of integral to return.
	 * @param ch			A single valid hexadecimal character. (Range: 0 - F)
	 * @returns				ReturnType
	 */
	template<std::integral ReturnType>
	inline static CONSTEXPR ReturnType hex_to_int(const char& ch)
	{
		if (isdigit(ch))
			return static_cast<ReturnType>(ch - '0');
		else if (ch >= 'a' && ch <= 'f')
			return static_cast<ReturnType>(ch - 'a') + static_cast<ReturnType>(10);
		else if (ch >= 'A' && ch <= 'F')
			return static_cast<ReturnType>(ch - 'A') + static_cast<ReturnType>(10);
		return static_cast<ReturnType>(0);
	}

	/**
	 * @brief				Convert a single hexadecimal value to an integral.
	 * @tparam ReturnType	Type of integral to return.
	 * @param hex_str		Hexadecimal value as a string.
	 *\n					Note that this CANNOT contain prefix characters like '#' or "0x"
	 * @returns				ReturnType
	 */
	template<std::integral ReturnType>
	inline static CONSTEXPR ReturnType hex_to_int(const std::string& hex_str) noexcept
	{
		if (hex_str.empty())
			return static_cast<ReturnType>(0);
		const auto base{ static_cast<ReturnType>(16) };
		ReturnType power{ 1 }, result{ 0 };
		for (auto ch{ hex_str.rbegin() }; ch != hex_str.rend(); ++ch) {
			if (const auto v{ hex_to_int<ReturnType>(*ch) }; v < base) {
				result += v * power;
				power = power * base;
			}
		}
		return result;
	}

	/**
	 * @brief		Convert a hexadecimal value to an RGB color value.
	 * @tparam T	Integral Type.
	 * @param hex	A hexadecimal value as a string.
	 *\n			Note that this CANNOT contain prefix characters like '#' or "0x"
	 * @returns		RGB<T>
	 */
	template<std::integral T = ColorT>
	inline static CONSTEXPR RGB<T> hex_to_rgb(const std::string& hex) noexcept
	{
		return{
			math::normalize(hex_to_int<T>(hex.substr(0ull, 2ull)), { 0, 255 }, { 0, 5 }),
			math::normalize(hex_to_int<T>(hex.substr(2ull, 2ull)), { 0, 255 }, { 0, 5 }),
			math::normalize(hex_to_int<T>(hex.substr(4ull, 2ull)), { 0, 255 }, { 0, 5 })
		};
	}

	/**
	 * @brief		Convert a hexadecimal string to an SGR color value.
	 * @tparam T	Integral Type.
	 * @param hex	A hexadecimal value as a string.
	 *\n			Note that this CANNOT contain prefix characters like '#' or "0x"
	 * @returns		T
	 *				| Value | Description   |
	 *				| ----- | ------------- |
	 *				| 0     | Invalid Input |
	 *				| 0-255 | Output Value  |
	 */
	template<std::integral T = ColorT>
	inline static CONSTEXPR T hex_to_sgr(const std::string& hex) noexcept
	{
		if (!hex.empty() && std::all_of(hex.begin(), hex.end(), ishexnum))
			return rgb_to_sgr(hex_to_rgb(hex));
		return 0;
	}

	/**
	 * @brief			Convert an integer value to hexadecimal.
	 * @tparam T		Integral Type.
	 * @param v			Input Value.
	 * @param uppercase	When true, the returned hexadecimal string will use uppercase letters rather than lowercase.
	 * @returns			std::string
	 */
	template<std::integral T = ColorT>
	inline static std::string int_to_hex(const T& v, const bool& uppercase = true) noexcept
	{
		std::ostringstream buffer;
		buffer << std::hex << std::setfill('0') << (uppercase ? std::uppercase : std::nouppercase) << v;
		return buffer.str();
	}

	/**
	 * @brief			Convert an RGB color value to hexadecimal.
	 * @tparam T		Integral Type.
	 * @param rgb		Input RGB Color Value.
	 * @param uppercase	When true, the returned hexadecimal string will use uppercase letters rather than lowercase.
	 * @returns			std::string
	 */
	template<std::integral T = ColorT>
	inline static std::string rgb_to_hex(const RGB<T>& rgb, const bool& uppercase = true) noexcept
	{
		using namespace std::string_literals;
		return{ "0x"s + int_to_hex<T>(rgb.template get<0>()) + int_to_hex<T>(rgb.template get<1>()) + int_to_hex<T>(rgb.template get<2>()) };
	}

	/**
	 * @brief			Convert an SGR color value to hexadecimal.
	 * @tparam T		Integral Type.
	 * @param sgr		Input SGR color value.
	 * @param uppercase	When true, the returned hexadecimal string will use uppercase letters rather than lowercase.
	 * @returns			std::string
	 */
	template<std::integral T = ColorT>
	inline static std::string sgr_to_hex(const T& sgr, const bool& uppercase = true) noexcept
	{
		return rgb_to_hex<T>(sgr_to_rgb<T>(sgr), uppercase);
	}

#pragma endregion Hexadecimal_Transformations

#pragma region ConversionParser

	template<std::integral T = ColorT>
	inline T parse_hex_to_sgr(std::string hex)
	{
		if (!hex.empty())
			return static_cast<T>(0);
		if (hex.at(0ull) == '#')
			hex = hex.substr(1ull);
		return hex_to_sgr<T>(hex);
	}

#pragma endregion ConversionParser
}