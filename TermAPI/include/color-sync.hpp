#pragma once
/**
 * @file	color-sync.hpp
 * @author	radj307
 * @brief	Provides lightweight ANSI color sequence synchronization objects that are a keyless alternative to color::palette from palette.hpp
 */
#include <setcolor.hpp>
#include <Message.hpp>

#include <var.hpp>

#include <concepts>

namespace color {
	/**
	 * @class					basic_sync
	 * @tparam TChar			Char type
	 * @tparam TCharTraits		Char traits type for TChar
	 * @tparam TAlloc			Allocator type for TChar
	 * @brief					Base color synchronization object.
	 */
	template<var::valid_char TChar, std::derived_from<std::char_traits<TChar>>TCharTraits = std::char_traits<TChar>, std::derived_from<std::allocator<TChar>> TAlloc = std::allocator<TChar>>
	class basic_sync {
		using seq_t = typename color::setcolor_seq<TChar, TCharTraits, TAlloc>;
		/// @brief	When this is true, color sequences are emitted by basic_sync methods; otherwise color sequences are disabled.
		bool _enable;
		/// @brief	The default reset sequence to use. This allows the user to reset formatting in addition to colors if needed.
		seq_t _reset_seq;

	public:
		/**
		 * @brief			Default Constructor
		 * @param enable	When true, color sequences are enabled; otherwise color sequences are disabled.
		 */
		constexpr basic_sync(const bool enable = true, const seq_t& reset_seq = seq_t::reset) : _enable{ enable }, _reset_seq{ reset_seq } {}

		/**
		 * @brief			Sets whether color sequences are emitted by this sync object or not.
		 * @param enable	When true, color sequences are enabled; otherwise color sequences are disabled.
		 */
		constexpr void setEnabled(const bool enable) noexcept { _enable = enable; }
		/**
		 * @brief			Gets whether color sequences are emitted by this sync object or not.
		 * @returns			true when color sequences are enabled; otherwise false.
		 */
		constexpr bool getEnabled() const noexcept { return _enable; }

		/// @returns	true when the sync object is enabled; otherwise false.
		constexpr operator bool() const noexcept { return _enable; }

		/**
		 * @brief				Sets the default reset sequence.
		 * @param reset_seq		The sequence to use when resetting the terminal.
		 */
		constexpr void setDefaultResetSequence(const seq_t& reset_seq) noexcept
		{
			_reset_seq = reset_seq;
		}
		/**
		 * @brief		Gets the default reset sequence.
		 * @returns		The default reset sequence that is currently in-use.
		 */
		constexpr seq_t getDefaultResetSequence() const noexcept
		{
			return _reset_seq;
		}

		/**
		 * @brief				Gets any number of streamable types as a single, concatenated sequence, or an empty placeholder if the sync object is disabled.
		 * @param sequences		Any number of types that have a valid overload for std::basic_stringstream<TChar, TCharTraits, TAlloc>>::operator<<.
		 * @returns				When the sync object is enabled, a sequence that is equal to all of the given sequences concatenated together, in order; otherwise an empty placeholder.
		 */
		template<var::streamable<std::basic_stringstream<TChar, TCharTraits, TAlloc>>... Ts> requires var::at_least_one<Ts...>
		constexpr seq_t operator()(Ts&&... sequences) const
		{
			return _enable ? seq_t{ ANSI::make_sequence<TChar, TCharTraits, TAlloc>(std::forward<Ts>(sequences)...) } : seq_t::placeholder;
		}
		/**
		 * @brief			Gets a sequence that sets the color of the terminal foreground or background to the specified Select Graphics Rendition (SGR) color code.
		 * @param sgr		An SGR code; see <https://en.wikipedia.org/wiki/ANSI_escape_code#SGR_(Select_Graphic_Rendition)_parameters>
		 * @param layer		The target layer to set to the given color.
		 * @returns			When the sync object is enabled, a sequence that sets the specified layer to the specified SGR color; otherwise an empty placeholder.
		 */
		template<std::integral T>
		constexpr seq_t operator()(const T sgr, const Layer layer = Layer::Foreground) const
		{
			return _enable ? seq_t{ sgr, layer } : seq_t::placeholder;
		}
		/**
		 * @brief				Gets a sequence that sets the terminal foreground/background to the closest approximation of the given RGB color value.
		 * @tparam RANGE_MAX	Specifies the maximum range boundary for each of the red/green/blue channels. The minimum is always 0.
		 *\n					The actual values are always scaled to be within (0-5); setting this to a higher number does not increase precision.
		 * @tparam TRed			Integral type to use for the red channel.
		 * @tparam TGreen		Integral type to use for the green channel.
		 * @tparam TBlue		Integral type to use for the blue channel.
		 * @param r				Red color channel value. This max not exceed RANGE_MAX, and must be greater than 0.
		 * @param g				Green color channel value. This max not exceed RANGE_MAX, and must be greater than 0.
		 * @param b				Blue color channel value. This max not exceed RANGE_MAX, and must be greater than 0.
		 * @param layer			The target layer to set to the given color.
		 * @returns				When the sync object is enabled, a sequence that sets the specified layer to the specified RGB color; otherwise an empty placeholder.
		 */
		template<size_t RANGE_MAX = 5ull, std::integral TRed, std::integral TGreen, std::integral TBlue>
		constexpr seq_t operator()(const TRed r, const TGreen g, const TBlue b, const Layer layer = Layer::Foreground) const
		{
			math::scale(r, std::make_pair(0, RANGE_MAX), std::make_pair(0, 5));
			return _enable ? seq_t{ r, g, b, layer } : seq_t::placeholder;
		}
		/**
		 * @brief			Gets a sequence that sets the terminal foreground/background to the closest approximation of the given RGB color value.
		 * @param rgb		Red/Green/Blue color channel values in a tuple.
		 * @param layer		The target layer to set to the given color.
		 * @returns			When the sync object is enabled, a sequence that sets the specified layer to the specified RGB color; otherwise an empty placeholder.
		 */
		template<size_t MAX = 5ull, std::integral TRed, std::integral TGreen, std::integral TBlue>
		constexpr seq_t operator()(const std::tuple<TRed, TGreen, TBlue>& rgb, const Layer layer = Layer::Foreground) const
		{
			return this->operator() < MAX > (std::get<0>(rgb), std::get<1>(rgb), std::get<2>(rgb), layer);
		}
		/**
		 * @brief			Resets the color of the specified termianl layer, or all terminal layers.
		 * @param layer		Optional target layer to reset; when this is left as the default std::nullopt, the default reset sequence is used.
		 * @returns			When the sync object is enabled, a sequence that resets the color of the specified layer(s) to default; otherwise an empty placeholder.
		 */
		constexpr seq_t operator()(const std::optional<Layer> layer = std::nullopt) const
		{
			if (!_enable) return seq_t::placeholder;
			else if (layer.has_value()) {
				if (layer.value() == Layer::Foreground)
					return seq_t::reset_f;
				else
					return seq_t::reset_b;
			}
			else return _reset_seq;
		}

	#	pragma region MessageHeaders
		/// @brief	Returns [DEBUG] header that uses colors only if the palette is enabled.
		term::Message get_debug() const noexcept { return term::get_debug(_enable, term::MessageMarginSize); }
		/// @brief	Returns [INFO] header that uses colors only if the palette is enabled.
		term::Message get_info() const noexcept { return term::get_info(_enable, term::MessageMarginSize); }
		/// @brief	Returns [LOG] header that uses colors only if the palette is enabled.
		term::Message get_log() const noexcept { return term::get_log(_enable, term::MessageMarginSize); }
		/// @brief	Returns [MSG] header that uses colors only if the palette is enabled.
		term::Message get_msg() const noexcept { return term::get_msg(_enable, term::MessageMarginSize); }
		/// @brief	Returns [WARN] header that uses colors only if the palette is enabled.
		term::Message get_warn() const noexcept { return term::get_warn(_enable, term::MessageMarginSize); }
		/// @brief	Returns [ERROR] header that uses colors only if the palette is enabled.
		term::Message get_error() const noexcept { return term::get_error(_enable, term::MessageMarginSize); }
		/// @brief	Returns [CRIT] header that uses colors only if the palette is enabled.
		term::Message get_crit() const noexcept { return term::get_crit(_enable, term::MessageMarginSize); }
		/// @brief	Returns [FATAL] header that uses colors only if the palette is enabled.
		term::Message get_fatal() const noexcept { return term::get_fatal(_enable, term::MessageMarginSize); }
		/// @brief	Returns an empty space header the same size as a normal message's indentation.
		term::Message get_placeholder() const noexcept { return term::placeholder; }
	#	pragma endregion MessageHeaders
	};

	/// @brief	Simple color synchronization object for narrow-width-char streams.
	using sync = basic_sync<char, std::char_traits<char>, std::allocator<char>>;
	/// @brief	Simple color synchronization object for narrow-width-char streams.
	using sync8_t = basic_sync<char8_t, std::char_traits<char8_t>, std::allocator<char8_t>>;

	/// @brief	Simple color synchronization object for wide-width-char streams.
	using wsync = basic_sync<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t>>;
	/// @brief	Simple color synchronization object for wide-width-char streams.
	using sync16_t = basic_sync<char16_t, std::char_traits<char16_t>, std::allocator<char16_t>>;

	/// @brief	Simple color synchronization object for double-wide-width-char streams.
	using sync32_t = basic_sync<char32_t, std::char_traits<char32_t>, std::allocator<char32_t>>;
}
