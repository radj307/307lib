/**
 * @file strlocale.hpp
 * @author radj307
 * @brief Contains objects, methods, and functions related to string/stream manipulation and std::locale
 */
#pragma once
#include <iostream>
#include <optional>
#include <locale>

namespace str {
	/**
	 * @struct SetLocale
	 * @brief Set the locale of a stream. If you want to apply a locale Facet, use struct SetLocaleFacet<> instead.
	 */
	struct SetLocale {
		std::locale _locale; ///< @brief The locale to set streams to.

		/**
		 * @brief Constructor meant to be used inline in an iostream's operator<< or operator>> chain.
		 * @param locale	- The locale to imbue the stream with.
		 */
		SetLocale(std::locale locale) : _locale{ std::move(locale) } {}

		/**
		 * @brief Retrieve the the internal locale.
		 * @returns const std::locale
		 */
		operator const std::locale() const { return _locale; }

		friend std::istream& operator>>(std::istream& is, SetLocale& obj)
		{
			obj._locale = is.imbue(obj.operator const std::locale());
			return is;
		}
		friend std::istream& operator>>(std::istream& is, const SetLocale& obj)
		{
			is.imbue(obj.operator const std::locale());
			return is;
		}
		friend std::ostream& operator<<(std::ostream& os, SetLocale& obj)
		{
			obj._locale = os.imbue(obj.operator const std::locale());
			return os;
		}
		friend std::ostream& operator<<(std::ostream& os, const SetLocale& obj)
		{
			os.imbue(obj.operator const std::locale());
			return os;
		}
	};

	/**
	 * @struct SetLocaleFacet
	 * @brief Set the locale of an iostream, and set a Facet at the same time.
	 * @tparam Facet	- Facet Type. A new object of this type will be constructed by the operator>> | operator<< functions.
	 */
	template<class Facet> struct SetLocaleFacet {
		std::optional<std::locale> _locale;

		/**
		 * @brief Default Constructor.
		 * @param locale	- Optional locale override. If this is null, the local machine's current locale will be used instead.
		 */
		SetLocaleFacet(std::optional<std::locale> locale = std::nullopt) : _locale{ std::move(locale) } {}

		operator std::locale() const { return _locale.value_or(std::locale{}); }

		friend std::istream& operator<<(std::istream& is, const SetLocaleFacet<Facet>& set)
		{
			is.imbue(std::locale{ set.operator std::locale(), new Facet() });
			return is;
		}
		friend std::ostream& operator<<(std::ostream& os, const SetLocaleFacet<Facet>& set)
		{
			os.imbue(std::locale{ set.operator std::locale(), new Facet() });
			return os;
		}
	};

	/**
	 * @namespace facet
	 * @brief Contains individual facet objects for use with the SetLocaleFacet function. For ready-to-use functions using the facet namespace, see below in namespace str.
	 */
	namespace facet {
		/**
		 * @class NumberGrouping
		 * @brief Inserts commas between groups of thousands in large digits. Ex: "1000" -> "1,000"
		 *\n	  Can be applied by using the SetLocaleFacet<NumberGrouping> functor.
		 */
		class NumberGrouping : public std::numpunct<char> {
		protected:
			virtual char do_thousands_sep() const override
			{
				return ',';
			}
			virtual std::string do_grouping() const override
			{
				return "\03";
			}
		};
	}

	/**
	 * @brief Placeholder function that does nothing. Intended for use in ternary expressions.
	 * @param os	- Target Output Stream
	 * @returns std::ostream&
	 */
	inline std::ostream& Placeholder(std::ostream& os)
	{
		return os;
	}

	/**
	 * @brief Automatically insert commas between thousands when printing numbers.
	 *\n	  Example: "1000000" => "1,000,000"
	 * @param os	- (implicit) Target Output Stream
	 * @returns std::ostream&
	 */
	inline std::ostream& NumberGrouping(std::ostream& os)
	{
		os << SetLocaleFacet<facet::NumberGrouping>();
		return os;
	}
}