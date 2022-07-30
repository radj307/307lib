/**
 * @file palette.hpp
 * @author radj307
 * @brief Contains the ColorPalette struct, which provides a convenient API for applying consistent colors throughout a program.
 */
#pragma once
#include <Segments.h>
#include <setcolor.hpp>
#include <Message.hpp>

#include <var.hpp>

#include <fstream>
#include <algorithm>
#include <map>

namespace term {
	/**
	 * @class		palette
	 * @brief		Associative binding container with additional helper methods & operators.
	 *\n			Maps color sequences to a templated key type.
	 *\n			Provides convenience methods for recalling color sequences by using keys.
	 *\n			Very useful when you want to control the usage of escape sequences throughout your program.
	 * @tparam TKey
	 */
	template<typename TKey, var::valid_char TChar = char, typename TCharTraits = std::char_traits<TChar>, typename TAlloc = std::allocator<TChar>>
	class palette {
	public:
		using key_type = TKey;
		using value_type = setcolor;
		using pair_type = std::pair<key_type, value_type>;
		using container_type = std::map<key_type, value_type>;
		using size_type = container_type::size_type;
		using iterator = container_type::iterator;
		using const_iterator = container_type::const_iterator;
	protected:
		container_type _palette;
		bool _enable{ true };
		ANSI::basic_sequence<TChar, TCharTraits, TAlloc> _reset_seq{ color::reset + color::reset_fmt };

		inline static ANSI::basic_sequence<TChar, TCharTraits, TAlloc> return_if_disabled(const char& c) { return{ std::string(1ull, c) }; }
		inline static ANSI::basic_sequence<TChar, TCharTraits, TAlloc> return_if_disabled(const ANSI::basic_sequence<TChar, TCharTraits, TAlloc>& seq) { return seq; }

	public:
#		pragma region Constructors
		/**
		 * @brief	Default Constructor
		 */
		palette() = default;

		/**
		 * @brief				Constructor
		 * @param palette		rvalue reference to a pre-existing palette container.
		 * @param enable		When true, escape sequences are enabled.
		 * @param reset_seq		The escape sequence used by the reset functions.
		 */
		palette(container_type&& palette, const bool& enable = true, const ANSI::basic_sequence<TChar, TCharTraits, TAlloc>& reset_seq = color::reset) : _palette{ std::move(palette) }, _enable{ enable }, _reset_seq{ reset_seq } {}

		/**
		 * @brief				Initializer List Constructor
		 * @param colors		Any number of color pairs.
		 */
		palette(std::initializer_list<pair_type>&& colors) : _palette{ std::move(colors.begin()), std::move(colors.end()) } {}
#		pragma endregion Constructors

#		pragma region Functions
		/**
		 * @brief	Check if the palette is currently enabled.
		 * @returns	bool
		 */
		constexpr bool enabled() const { return _enable; }

		/**
		 * @brief	Check if the palette is currently enabled.
		 *\n		This function exists for backwards-compatibility with previous versions of palette.hpp
		 * @returns	bool
		 */
		constexpr bool isActive() const { return enabled(); }

		/**
		 * @brief			Enable or disable the palette.
		 * @param enable	When true, the palette will be enabled.
		 * @returns			bool
		 *\n				The previous enabled state.
		 */
		constexpr bool setEnabled(const bool& enable)
		{
			const auto copy{ _enable };
			_enable = enable;
			return copy;
		}

		/**
		 * @brief			Enable or disable the palette.
		 *\n				This function exists for backwards-compatibility with previous versions of palette.hpp
		 * @param enable	When true, the palette will be enabled.
		 * @returns			bool
		 *\n				The previous enabled state.
		 */
		constexpr bool setActive(const bool& active) { return setEnabled(active); }

		/// @brief	Enable this color palette.
		constexpr void enable() { setEnabled(true); }
		/// @brief	Disable this color palette.
		constexpr void disable() { setEnabled(false); }

		/**
		 * @brief		Change the default sequence used to reset terminal colors & formatting, when an override isn't specified.
		 * @param seq	The sequence to use as the default reset sequence.
		 * @returns		ANSI::basic_sequence<TChar, TCharTraits, TAlloc>
		 *\n			The previous default reset sequence.
		 */
		virtual ANSI::basic_sequence<TChar, TCharTraits, TAlloc> setDefaultResetSequence(const ANSI::basic_sequence<TChar, TCharTraits, TAlloc>& seq)
		{
			const auto copy{ _reset_seq };
			_reset_seq = seq;
			return copy;
		}

		/**
		 * @brief		Get the default sequence used to reset terminal colors & formatting.
		 * @returns		ANSI::basic_sequence<TChar, TCharTraits, TAlloc>
		 *\n			The current default reset sequence.
		 */
		virtual ANSI::basic_sequence<TChar, TCharTraits, TAlloc> getDefaultResetSequence() const { return _reset_seq; }

		/**
		 * @brief		Check if a given key exists in the palette.
		 *\n			This function exists for backwards-compatibility with previous versions of palette.hpp
		 * @param key	The key to check for.
		 * @returns		bool
		 */
		virtual bool key_exists(key_type&& key) const { return contains(std::forward<key_type>(key)); }

#		pragma region ContainerPassthroughFunctions
		/// @brief	Get an iterator to the specified key if it exists, or end if it doesn't.
		virtual const_iterator find(key_type&& key) const { return _palette.find(std::forward<key_type>(key)); }
		/// @brief	Get an iterator for the beginning of the palette container.
		virtual const_iterator begin() const { return _palette.begin(); }
		/// @brief	Get an iterator for the end of the palette container.
		virtual const_iterator end() const { return _palette.end(); }
		/// @brief	Check if the palette has any registered color keys.
		virtual bool empty() const { return _palette.empty(); }
		/// @brief	Get the number of registered color keys.
		virtual size_type size() const { return _palette.size(); }
		/// @brief	Check if the palette contains the specified color key.
		virtual bool contains(key_type&& key) const { return _palette.contains(std::forward<key_type>(key)); }
		/// @brief	Insert a new color key pair into the palette if one doesn't already exist.
		auto&& insert(pair_type&& pr) { return _palette.insert(std::forward<pair_type>(pr)); }
		/// @brief	Insert a new color & key into the palette, or assign a key to the specified color if it already exists.
		auto&& insert_or_assign(key_type&& key, value_type&& value) { return _palette.insert_or_assign(std::forward<key_type>(key), std::forward<value_type>(value)); }
#		pragma endregion ContainerPassthroughFunctions
#		pragma endregion Functions

#		pragma region SequenceGetters
		/**
		 * @brief		Return a sequence that will set the current console output color to the one associated with a specified key.
		 *\n			If the palette is enabled and the key doesn't exist, nothing happens and an empty placeholder is returned.
		 *\n			If the palette is disabled, nothing happens.
		 * @param key	The key associated with the desired color.
		 * @returns		ANSI::basic_sequence<TChar, TCharTraits, TAlloc>
		 */
		ANSI::basic_sequence<TChar, TCharTraits, TAlloc> set(key_type&& key) const noexcept
		{
			if (_enable)
				if (const auto it{ find(std::forward<key_type>(key)) }; it != end())
					return it->second;
			return setcolor::placeholder;
		}

		/**
		 * @brief				Return a sequence that will set the current console output color to the one associated with a specified key.
		 *\n					If the palette is enabled and the key doesn't exist, nothing happens and an empty placeholder is returned.
		 *\n					If the palette is disabled, the string or char specified by "if_disabled" is returned instead.
		 * @param key			The key associated with the desired color.
		 * @param if_disabled	Any string or character to return instead if the palette is currently disabled.
		 * @returns				ANSI::basic_sequence<TChar, TCharTraits, TAlloc>
		 */
		template<var::any_same_or_convertible<std::string, char> T>
		ANSI::basic_sequence<TChar, TCharTraits, TAlloc> set_or(key_type&& key, const T& if_disabled) const noexcept
		{
			if (_enable)
				return set(std::forward<key_type>(key));
			return return_if_disabled(if_disabled);
		}

		/**
		 * @brief		Return a sequence that will reset the current console output color using the default reset sequence.
		 *\n			If the palette is disabled, nothing happens.
		 * @returns		ANSI::basic_sequence<TChar, TCharTraits, TAlloc>
		 */
		ANSI::basic_sequence<TChar, TCharTraits, TAlloc> reset() const noexcept
		{
			if (_enable)
				return _reset_seq;
			return setcolor::placeholder;
		}

		/**
		 * @brief				Return a sequence that will reset the current console output color using the default reset sequence.
		 *\n					If the palette is disabled, the string or char specified by "if_disabled" is returned instead.
		 * @param if_disabled	Any string or character to return instead if the palette is currently disabled.
		 * @returns				ANSI::basic_sequence<TChar, TCharTraits, TAlloc>
		 */
		template<var::any_same_or_convertible<std::string, char> T>
		ANSI::basic_sequence<TChar, TCharTraits, TAlloc> reset_or(const T& if_disabled) const noexcept
		{
			if (_enable)
				return reset();
			return return_if_disabled(if_disabled);
		}

		/**
		 * @brief		Return a sequence that will reset the current console output color using the default reset sequence, and set it to the one associated with a specified key.
		 *\n			If the palette is enabled and the key doesn't exist, only the reset sequence is returned.
		 *\n			If the palette is disabled, nothing happens.
		 * @param key	The key associated with the desired color.
		 * @returns		ANSI::basic_sequence<TChar, TCharTraits, TAlloc>
		 */
		ANSI::basic_sequence<TChar, TCharTraits, TAlloc> reset(key_type&& key) const noexcept
		{
			if (_enable) {
				if (const auto it{ find(std::forward<key_type>(key)) }; it != end())
					return ANSI::make_sequence(_reset_seq, it->second);
				return _reset_seq;
			}
			return setcolor::placeholder;
		}

		/**
		 * @brief				Return a sequence that will reset the current console output color using the default reset sequence, and set it to the one associated with a specified key.
		 *\n					If the palette is enabled and the key doesn't exist, only the reset sequence is returned.
		 *\n					If the palette is disabled, the string or char specified by "if_disabled" is returned instead.
		 * @param key			The key associated with the desired color.
		 * @param if_disabled	Any string or character to return instead if the palette is currently disabled.
		 * @returns				ANSI::basic_sequence<TChar, TCharTraits, TAlloc>
		*/
		template<var::any_same_or_convertible<std::string, char> T>
		ANSI::basic_sequence<TChar, TCharTraits, TAlloc> reset_or(key_type&& key, const T& if_disabled) const noexcept
		{
			if (_enable)
				return reset(std::forward<key_type>(key));
			return return_if_disabled(if_disabled);
		}


		/**
		 * @brief		Return a sequence that will set the current console output color to the one associated with a specified key.
		 *\n			If the palette is enabled and the key doesn't exist, nothing happens and an empty placeholder is returned.
		 *\n			If the palette is disabled, nothing happens.
		 * @param key	The key associated with the desired color.
		 * @returns		ANSI::basic_sequence<TChar, TCharTraits, TAlloc>
		 */
		ANSI::basic_sequence<TChar, TCharTraits, TAlloc> operator[](key_type&& key) const noexcept { return set(std::forward<key_type>(key)); }

		/**
		 * @brief		Return a sequence that will set the current console output color to the one associated with a specified key.
		 *\n			If the palette is enabled and the key doesn't exist, nothing happens and an empty placeholder is returned.
		 *\n			If the palette is disabled, nothing happens.
		 * @param key	The key associated with the desired color.
		 * @returns		ANSI::basic_sequence<TChar, TCharTraits, TAlloc>
		 */
		ANSI::basic_sequence<TChar, TCharTraits, TAlloc> operator()(key_type&& key) const noexcept { return set(std::forward<key_type>(key)); }

		/**
		 * @brief				Return a sequence that will set the current console output color to the one associated with a specified key.
		 *\n					If the palette is enabled and the key doesn't exist, nothing happens and an empty placeholder is returned.
		 *\n					If the palette is disabled, the string or char specified by "if_disabled" is returned instead.
		 * @param key			The key associated with the desired color.
		 * @param if_disabled	Any string or character to return instead if the palette is currently disabled.
		 * @returns				ANSI::basic_sequence<TChar, TCharTraits, TAlloc>
		 */
		ANSI::basic_sequence<TChar, TCharTraits, TAlloc> operator()(key_type&& key, auto&& if_disabled) const noexcept { return set_or(std::forward<key_type>(key), std::forward<decltype(if_disabled)>(if_disabled)); }

		/**
		 * @brief		Return a sequence that will reset the current console output color using the default reset sequence.
		 *\n			If the palette is disabled, nothing happens.
		 * @returns		ANSI::basic_sequence<TChar, TCharTraits, TAlloc>
		 */
		ANSI::basic_sequence<TChar, TCharTraits, TAlloc> operator()() const noexcept { return reset(); }

		/**
		 * @brief				Return a sequence that will reset the current console output color using the default reset sequence.
		 *\n					If the palette is disabled, the string or char specified by "if_disabled" is returned instead.
		 * @param if_disabled	Any string or character to return instead if the palette is currently disabled.
		 * @returns				ANSI::basic_sequence<TChar, TCharTraits, TAlloc>
		 */
		ANSI::basic_sequence<TChar, TCharTraits, TAlloc> operator()(auto&& if_disabled) const noexcept { return reset_or(if_disabled); }
#		pragma endregion SequenceGetters

#		pragma region MessageHeaders
		/// @brief	Returns [DEBUG] header that uses colors only if the palette is enabled.
		Message get_debug() const noexcept { return term::get_debug(_enable, MessageMarginSize); }
		/// @brief	Returns [INFO] header that uses colors only if the palette is enabled.
		Message get_info() const noexcept { return term::get_info(_enable, MessageMarginSize); }
		/// @brief	Returns [LOG] header that uses colors only if the palette is enabled.
		Message get_log() const noexcept { return term::get_log(_enable, MessageMarginSize); }
		/// @brief	Returns [MSG] header that uses colors only if the palette is enabled.
		Message get_msg() const noexcept { return term::get_msg(_enable, MessageMarginSize); }
		/// @brief	Returns [WARN] header that uses colors only if the palette is enabled.
		Message get_warn() const noexcept { return term::get_warn(_enable, MessageMarginSize); }
		/// @brief	Returns [ERROR] header that uses colors only if the palette is enabled.
		Message get_error() const noexcept { return term::get_error(_enable, MessageMarginSize); }
		/// @brief	Returns [CRIT] header that uses colors only if the palette is enabled.
		Message get_crit() const noexcept { return term::get_crit(_enable, MessageMarginSize); }
		/// @brief	Returns [FATAL] header that uses colors only if the palette is enabled.
		Message get_fatal() const noexcept { return term::get_fatal(_enable, MessageMarginSize); }
		/// @brief	Returns an empty space header the same size as a normal message's indentation.
		Message get_placeholder() const noexcept { return term::placeholder; }
#		pragma endregion MessageHeaders

#		pragma region FileIO
		// ENUM
		friend std::ofstream& operator<<(std::ofstream& ofs, const palette<key_type>& p) requires (std::is_enum_v<key_type>)
		{
			for (const auto& [key, color] : p._palette)
				ofs << std::to_string(static_cast<int>(key)) << "=" << color << '\n';
			return ofs;
		}
		// NON-ENUM
		friend std::ofstream& operator<<(std::ofstream& ofs, const palette<key_type>& p) requires (!std::is_enum_v<key_type>&& var::Streamable<key_type, std::ofstream>)
		{
			for (const auto& [key, color] : p._palette)
				ofs << key << "=" << color << '\n';
			return ofs;
		}
		// ENUM
		friend std::ifstream& operator>>(std::ifstream& ifs, palette<key_type>& p) requires (std::is_enum_v<key_type>)
		{
			for (std::string lnbuf; std::getline(ifs, lnbuf, '\n'); ) {
				if (!lnbuf.empty()) {
					lnbuf.erase(std::remove_if(lnbuf.begin(), lnbuf.end(), isspace), lnbuf.end()); // remove spaces
					if (auto eq{ lnbuf.find('=') }; eq != std::string::npos) {
						if (const auto keystr{ lnbuf.substr(0ull, eq) }; std::all_of(keystr.begin(), keystr.end(), isdigit)) {
							p.insert_or_assign(static_cast<key_type>(std::stoi(keystr)), setcolor{ lnbuf.substr(eq + 1ull) });
						}
						else continue;
					}
				}
			}
			return ifs;
		}
		// NON-ENUM
		friend std::ifstream& operator>>(std::ifstream& ifs, palette<key_type>& p) requires (!std::is_enum_v<key_type>&& var::Streamable<key_type, std::ifstream>)
		{
			for (std::string lnbuf; std::getline(ifs, lnbuf, '\n'); ) {
				if (!lnbuf.empty()) {
					lnbuf.erase(std::remove_if(lnbuf.begin(), lnbuf.end(), isspace), lnbuf.end()); // remove spaces
					if (auto eq{ lnbuf.find('=') }; eq != std::string::npos) {
						p.insert_or_assign(static_cast<key_type>(lnbuf.substr(0ull, eq)), setcolor{ lnbuf.substr(eq + 1ull) });
					}
				}
			}
			return ifs;
		}
#		pragma endregion FileIO
	};
}

namespace color { using term::palette; }
