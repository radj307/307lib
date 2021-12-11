/**
 * @file setcolor.hpp
 * @author radj307
 * @brief Contains the setcolor struct, which can be used inline with ostream's operator<< to set text/background color, as well as apply formatting like bold, invert, and underline.
 */
#pragma once
#include <format-functions.hpp>
#include <FormatFlag.hpp>
#include <Layer.hpp>
#include <sstream>
namespace color {
	/**
	 * @brief Builds the escape sequence needed to change console colors.
	 * @param color			- The color value to set.
	 * @param foreground	- When true, changes the color of the foreground (text) instead of the background.
	 * @returns std::string
	 */
	inline std::string makeColorSequence(const short& color, const Layer& layer)
	{
		std::stringstream ss;
		using namespace ANSI;
		ss << ESC << CSI << layer << ';' << color << END;
		return ss.str();
	}

	/**
	 * @struct set
	 * @brief Designed as an inline ANSI-escape-sequence-based color changer. Example: std::cout << color::set(color::red);
	 *\n	NOTE: If you're using Windows, you need to enable virtual terminal sequences by using the following line: (requires windows TermAPI lib or custom implementation)
	 *\n	std::cout << sys::term::EnableANSI;
	 */
	struct setcolor {
	private:
		std::string _seq; ///< @brief The escape sequence to set colors.
		ColorFormat _format; ///< @brief Stores information about bold/underline/invert

	public:
		/**
		 * @brief				Constructor that automatically generates an escape sequence with the given parameters.
		 * @param color			A number within the terminal's color range.
		 * @param layer			Which layer to apply the color to. (FOREGROUND/BACKGROUND)
		 * @param format		Which format flags to apply, if any. You can use the bitwise OR operator to combine multiple flags.
		 */
		setcolor(const short color, const Layer layer = Layer::FOREGROUND, const FormatFlag& format = FormatFlag::NONE) : _seq{ std::move(makeColorSequence(color, layer)) }, _format{ format } {}
		/**
		 * @brief				Constructor that automatically generates an escape sequence with the given parameters, but always applies the color to the foreground.
		 * @param color			A number within the terminal's color range. (up to 255)
		 * @param format		Which format flags to apply, if any. You can use the bitwise OR operator to combine multiple flags.
		 */
		setcolor(const short color, const FormatFlag format) : _seq{ std::move(makeColorSequence(color, Layer::FOREGROUND)) }, _format{ format } {}
		/**
		 * @brief				Constructor that accepts an escape sequence string.
		 * @param color_seq		The full ANSI escape sequence, stored in a string variable. This is simply inserted into whichever output stream you target.
		 * @param format		Which format flags to apply, if any. You can use the bitwise OR operator to combine multiple flags.
		 */
		setcolor(std::string color_seq, const FormatFlag& format = FormatFlag::NONE) : _seq{ std::move(color_seq) }, _format{ format } {}

		/**
		 * @brief		Retrieve the escape sequence as a string.
		 * @returns		std::string
		 */
		operator std::string() const;

		/**
		 * @brief Retrieve the current format flags.
		 * @returns FormatFlag
		 */
		virtual ColorFormat getFormat() const;
		/**
		 * @brief Set the format flag to a new value.
		 * @param newFormat		- Replaces the current formatting flags.
		 * @returns FormatFlag	- Previous format flags
		 */
		virtual ColorFormat setFormat(const FormatFlag& newFormat);
		virtual ColorFormat addFormat(const FormatFlag& modFormat);
		virtual ColorFormat removeFormat(const FormatFlag& modFormat);

		// Comparison Operators
		virtual bool operator==(const setcolor& o) const;
		virtual bool operator!=(const setcolor& o) const;

		// Output Stream insertion operator
		friend std::ostream& operator<<(std::ostream& os, const setcolor& obj)
		{
			os << obj._seq;
			if (obj._format != FormatFlag::NONE) {
				if (obj._format == FormatFlag::BOLD)
					os << bold();
				if (obj._format == FormatFlag::RESET_BOLD)
					os << reset_bold();
				if (obj._format == FormatFlag::UNDERLINE)
					os << underline();
				if (obj._format == FormatFlag::RESET_UNDERLINE)
					os << reset_underline();
				if (obj._format == FormatFlag::INVERT)
					os << invert();
				if (obj._format == FormatFlag::RESET_INVERT)
					os << reset_invert();
			}
			return os;
		}
	};
	/// @brief This setcolor instance can be used as a placeholder, when operator<< is called, nothing will be inserted. (Note that operator<< will still cause certain stream flags/facets to be reset!)
	static const setcolor setcolor_placeholder{ std::string(""), FormatFlag::NONE };
	struct textcolor : setcolor { textcolor(const short color, FormatFlag format = FormatFlag::NONE) : setcolor(color, Layer::FOREGROUND, std::move(format)) {} };

}