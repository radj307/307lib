/**
 * @file	basic_colors.hpp
 * @author	radj307
 * @brief	Contains lighter alternatives to the `palette` object provided in <palette.hpp> that provide the same global color toggle feature, and optionally includes the ability to store colors with keys as well.
 */
#pragma once
#include <sysarch.h>
#include <Sequence.hpp>
#include <Segments.h>
#include <setcolor.hpp>
#include <Message.hpp>

#include <var.hpp>

#include <concepts>
#include <iostream>
#include <map>

namespace term {
	/**
	 * @class					basic_colors
	 * @brief					A simple color palette object that allows some useful features like a global color toggle, without the extra bulk of a key:color map.
	 *\n						Custom functionality can be added by deriving from this object as most methods are marked virtual.
	 * @attention				This object's destructor automatically emits a reset escape sequence to the appropriate iostream for STDOUT. To disable this feature, set active to false before the object goes out-of-scope.
	 * @details					Methods for color sequences are not provided by this object directly, use `operator()` instead.
	 * @tparam SequenceType		Determines the character width used by escape sequence strings.
	 */
	template<var::any_same_or_convertible<ANSI::Sequence, ANSI::wSequence> SequenceType>
	class basic_colors {
	public:
		/// @brief	Sequence Type
		using seq = SequenceType;
		/// @brief	setcolor object using the appropriate sequence type.
		using setcolor = color::setcolor_seq<seq>;

		/// @brief	When true, color sequences may be emitted. This is a mutable boolean, so it can be changed even if the object is declared `const`.
		mutable bool active{ true };

	protected:
		/// @brief	True when the templated sequence type uses wide chars.
		static constexpr const bool isWideChar{ std::same_as<seq, ANSI::wSequence> };
		/// @brief	Defines the color reset sequence to use when resetting terminal colors.
		setcolor reset{ color::setcolor_seq<seq>::reset };
		/// @brief	Defines the format reset sequence to use when resetting text formatting.
		setcolor reset_fmt{ color::setcolor_seq<seq>::reset_fmt };
		/// @brief	Defines the placeholder sequence to use instead when active is false.
		setcolor placeholder{ color::setcolor_seq<seq>::placeholder };

	public:
		/// @brief	Default Constructor
		constexpr basic_colors() {}
		/// @brief	Destructor emits a single reset sequence to STDOUT if active is true.
		~basic_colors()
		{
			if (active) {
				if constexpr (isWideChar)
					std::wcout << reset;
				else
					std::cout << reset;
			}
		}

		/**
		 * @brief				Resets colors back to default.
		 * @attention			This returns a placeholder if not active.
		 * @param reset_format	When true, additional reset sequences are included for bold/underlined/inverted text formatting. (This is only checked when active)
		 * @returns				Sequence type that resets terminal colors to their default values.
		 */
		WINCONSTEXPR setcolor operator()(const bool& reset_format = false) const noexcept
		{
			return active
				? (
					reset_format
					? reset + reset_fmt
					: reset
				  )
				: placeholder;
		}
		/**
		 * @brief					Forwards the given color escape sequence if colors are enabled, otherwise a placeholder is returned.
		 * @attention				This returns a placeholder if not active.
		 * @param color_sequence	The color sequence to emit if colors are enabled.
		 * @returns					seq
		 */
		WINCONSTEXPR setcolor operator()(const setcolor& color_sequence) const noexcept
		{
			return active ? color_sequence : placeholder;
		}

		/// @brief	Returns [DEBUG] header that uses colors only if the palette is enabled.
		term::Message get_debug() const noexcept { return term::get_debug(active, MessageMarginSize); }
		/// @brief	Returns [INFO] header that uses colors only if the palette is enabled.
		term::Message get_info() const noexcept { return term::get_info(active, MessageMarginSize); }
		/// @brief	Returns [LOG] header that uses colors only if the palette is enabled.
		term::Message get_log() const noexcept { return term::get_log(active, MessageMarginSize); }
		/// @brief	Returns [MSG] header that uses colors only if the palette is enabled.
		term::Message get_msg() const noexcept { return term::get_msg(active, MessageMarginSize); }
		/// @brief	Returns [WARN] header that uses colors only if the palette is enabled.
		term::Message get_warn() const noexcept { return term::get_warn(active, MessageMarginSize); }
		/// @brief	Returns [ERROR] header that uses colors only if the palette is enabled.
		term::Message get_error() const noexcept { return term::get_error(active, MessageMarginSize); }
		/// @brief	Returns [CRIT] header that uses colors only if the palette is enabled.
		term::Message get_crit() const noexcept { return term::get_crit(active, MessageMarginSize); }
		/// @brief	Returns [FATAL] header that uses colors only if the palette is enabled.
		term::Message get_fatal() const noexcept { return term::get_fatal(active, MessageMarginSize); }
		/// @brief	Returns an empty space header the same size as a normal message's indentation.
		term::Message get_placeholder() const noexcept { return term::placeholder; }
	};
	/// @brief	Color sequences using char
	using colors = basic_colors<ANSI::Sequence>;
	/// @brief	Color sequences using wchar_t
	using wcolors = basic_colors<ANSI::wSequence>;

	/**
	 * @class					basic_colorpalette
	 * @brief					A simple color palette object that allows some useful features like a global color toggle and global color synchronization.
	 *\n						Custom functionality can be added by deriving from this object as most methods are marked virtual.
	 * @details					This is an alternative to the term::palette object that is based on the term::basic_colors object instead.
	 *\n						Use `operator[]` to access the colormap. Use `operator()` to access the default basic_colors operators.
	 * @attention				This object's destructor automatically emits a reset escape sequence to the appropriate iostream for STDOUT. To disable this feature, set active to false before the object goes out-of-scope.
	 * @tparam KeyType			Any integral indexer type to use as the palette key type.
	 * @tparam SequenceType		Determines the character width used by escape sequence strings.
	 */
	template<std::integral KeyType, var::any_same_or_convertible<ANSI::Sequence, ANSI::wSequence> SequenceType>
	class basic_colorpalette : public basic_colors<SequenceType> {
	public:
		using base = basic_colors<SequenceType>;
		using key_t = KeyType;
		using seq = base::seq;
		using setcolor = base::setcolor;

		// inherit constructors
		using base::base;

	protected:
		std::map<key_t, seq> map;

	public:
		/**
		 * @brief
		 * @param key	The key type associated with the desired
		 * @returns		seq
		 */
		seq operator[](key_t&& key) const
		{
			return this->active ? map[key] : placeholder;
		}
	};
	template<std::integral KeyType>
	using colorpalette = basic_colorpalette<KeyType, ANSI::Sequence>;
	template<std::integral KeyType>
	using wcolorpalette = basic_colorpalette<KeyType, ANSI::wSequence>;
}
namespace color {
	template<var::any_same_or_convertible<ANSI::Sequence, ANSI::wSequence> SequenceType>
	/// @brief	Provides a lightweight implementation of a global color toggle using the term::basic_colors object.
	using basic_colors = term::basic_colors<SequenceType>;
	/// @brief	Provides a lightweight implementation of a global color toggle using the term::basic_colors object.
	using term::colors;
	/// @brief	Provides a lightweight implementation of a global color toggle using the term::basic_colors object.
	using term::wcolors;

	template<std::integral KeyType, var::any_same_or_convertible<ANSI::Sequence, ANSI::wSequence> SequenceType>
	/// @brief	A simple color palette object that allows some useful features like a global color toggle and global color synchronization.
	using basic_colorpalette = term::basic_colorpalette<KeyType, SequenceType>;
	/// @brief	A simple color palette object that allows some useful features like a global color toggle and global color synchronization.
	using term::colorpalette;
	/// @brief	A simple color palette object that allows some useful features like a global color toggle and global color synchronization.
	using term::wcolorpalette;
}
