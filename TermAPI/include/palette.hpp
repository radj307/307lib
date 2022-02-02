/**
 * @file palette.hpp
 * @author radj307
 * @brief Contains the ColorPalette struct, which provides a convenient API for applying consistent colors throughout a program.
 *
 *	# Example Implementation: #
 *
 *	enum class MyKeyType {
 *		TYPE1,
 *		TYPE2,
 *		TYPE3,
 *	}
 *
 *	using Palette = color::ColorPalette<MyKeyType>;
 *
 *	Palette myPalette{
 *	{
 *		{ MyKeyType::TYPE1, color::red },
 *		{ MyKeyType::TYPE2, { color::green, color::Layer::FOREGROUND, color::FormatFlags::BOLD | color::FormatFlags::UNDERLINE } },
 *		{ MyKeyType::TYPE3, color::blue },
 *	}
 *	};
 *
 *	void example()
 *	{
 *		std::cout << myPalette(MyKeyType::TYPE1) << "red text" << color::reset;
 *	}
 */
#pragma once
#include <Segments.h>
#include <setcolor.hpp>
#include <make_exception.hpp>

#include <unordered_map>

namespace term {
	/**
	 * @struct palette
	 * @brief Contains a key-color map and functions to set terminal colors inline.
	 * @tparam KeyType	- The type of key to use as an identifier for each color. An enum of type char is recommended as each element only uses 1 byte.
	 */
	template<typename KeyType>
	class palette {
	public:
		using PaletteType = std::unordered_map<KeyType, setcolor>;
	private:
		PaletteType _palette;
		bool _isActive{ false }; ///< @brief When false, a blank setcolor placeholder will always be returned from the set() function. This can be used to programmatically enable/disable output colors depending on need.
		ANSI::Sequence _default_reset_seq{ color::reset };


	public:
		palette() = default;
		palette(PaletteType palette) : _palette{ std::move(palette) }, _isActive{ true } {}
		template<class... VT>
		palette(VT... color_pairs) : _palette{ std::move(color_pairs)... }, _isActive{ true } {}

		/**
		 * @brief Retrieve the reference of this palette's isActive boolean, allowing it to be modified.
		 * @returns bool&
		 */
		constexpr bool& isActive() { return _isActive; }

		/**
		 * @brief Function that
		 * @param new_state	- When true, the palette will be set to active.
		 * @returns bool
		 */
		constexpr bool setActive(const bool& new_state)
		{
			const auto copy{ _isActive };
			_isActive = new_state;
			return copy;
		}

		/**
		 * @brief Set this palette as active or inactive.
		 * @param active	- When true, palette is active.
		 * @returns ColorPalette<KeyType>&
		 */
		constexpr auto operator=(const bool& active)
		{
			_isActive = active;
			return *this;
		}
		/**
		 * @brief Reset this palette's key-color map.
		 * @param palette	- New key-color map
		 * @returns ColorPalette<KeyType>&
		 */
		constexpr auto operator=(const PaletteType& palette)
		{
			_palette = palette;
			return *this;
		}

		/**
		 * @brief Check if a given key exists in the palette.
		 * @param key	- Key to check for.
		 * @returns bool
		 */
		virtual bool key_exists(const KeyType& key) const
		{
			return _palette.contains(key);
		}

		/**
		 * @brief		Change the default sequence used to reset terminal colors & formatting, when an override isn't specified.
		 * @param seq	The sequence to use as the default reset sequence.
		 * @returns		ANSI::Sequence
		 *\n			The previous default reset sequence.
		 */
		ANSI::Sequence setDefaultResetSequence(const ANSI::Sequence& seq)
		{
			const auto copy{ _default_reset_seq };
			_default_reset_seq = seq;
			return copy;
		}

		/**
		 * @brief		Get the default sequence used to reset terminal colors & formatting.
		 * @returns		ANSI::Sequence
		 *\n			The current default reset sequence.
		 */
		ANSI::Sequence getDefaultResetSequence() const
		{
			return _default_reset_seq;
		}

		/**
		 * @brief Retrieve the mapped color setter functor for the given key when active, otherwise returns a placeholder.
		 * @param key	- Key associated with the target color.
		 * @returns setcolor
		 * @throws std::exception
		 */
		virtual setcolor set(const KeyType& key, const bool& quoteWhenInactive = false) const noexcept(false)
		{
			if (key_exists(key)) {
				return _isActive ? _palette.at(key) : (quoteWhenInactive ? setcolor("\"") : setcolor_placeholder);
			}
			if constexpr (var::Streamable<KeyType>)
				throw make_exception("set(KeyType) failed:  Key not found: \"", key, "\"!");
			throw make_exception("set(KeyType) failed:  Key not found!");
		}

		/**
		 * @brief	Equivalent of the color::reset function when active, however when not active, this function will return a placeholder to prevent escape sequence usage.
		 * @returns setcolor
		 */
		virtual setcolor reset(const std::optional<ANSI::Sequence>& reset_sequence = std::nullopt, const bool& quoteWhenInactive = false) const noexcept(false)
		{
			return _isActive ? setcolor{ reset_sequence.value_or(_default_reset_seq) } : (quoteWhenInactive ? setcolor("\"") : setcolor_placeholder);
		}
		/**
		 * @brief	Reset the current terminal color, then set it to a new value.
		 * @returns	setcolor
		 */
		virtual setcolor reset(const KeyType& key, const std::optional<ANSI::Sequence>& reset_sequence = std::nullopt, const bool& quoteWhenInactive = false) const noexcept(false)
		{
			return _isActive ? setcolor{ reset_sequence.value_or(_default_reset_seq) + _palette.at(key).operator ANSI::Sequence()} : (quoteWhenInactive ? setcolor("\"") : setcolor_placeholder);
		}

		/**
		 * @brief	Retrieve the mapped color setter functor for the given key. You can use this with output stream operator<< as an inline console color changer.
		 * @returns setcolor
		 */
		virtual setcolor operator()(const KeyType& key) const { return set(key); }
		virtual setcolor operator()() const { return reset(); }
		explicit operator PaletteType() const { return _palette; }
	};
}

namespace color {
	using term::palette;
}