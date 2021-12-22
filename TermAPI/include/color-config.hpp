/**
 * @file	color-config.hpp
 * @author	radj307
 * @brief	Color config is a relatively simple alternative to palette.hpp that uses files to store & read color values.
 *\n		Configs have many of the same functionality as a palette, but keys are always the std::string type.
 *\n		Configs have built-in conversion functions to create a palette from a config, and vice-versa.
 */
#pragma once
#include <color-format.hpp>
#include <color-transform.hpp>
#include <setcolor.hpp>
#include <palette.hpp>

#include <make_exception.hpp>
#include <fileio.hpp>
#include <fileutil.hpp>
#include <TokenizerBase.hpp>
#include <TokenParserBase.hpp>

#include <string>
#include <unordered_map>
#include <functional>
#include <tuple>
#include <ostream>

namespace color {

	class Config;

	/**
	 * @namespace	fileext
	 * @brief		Namespace containing file library extensions.
	 */
	namespace fileext { // Overload filelib's tokenizer objects for the color config file format
		using namespace token;
		// Tokenizes the given stringstream rvalue
		class ConfigTokenizer : public TokenizerBase {
		public:
			ConfigTokenizer(std::stringstream&& buffer) : TokenizerBase(std::forward<std::stringstream>(buffer)) {}
			Token getNext() override
			{
				const auto ch{ getch() };
				switch (get_lexeme(ch)) {
				case LEXEME::POUND: // Beginning of a hex color value
					return Token{ getsimilar(LEXEME::DIGIT, LEXEME::LETTER_UPPERCASE, LEXEME::LETTER_LOWERCASE), TokenType::NUMBER };
				case LEXEME::LETTER_UPPERCASE: [[fallthrough]];
				case LEXEME::LETTER_LOWERCASE: // Key name
					return Token{ std::string(1ull, ch) + getsimilar(LEXEME::LETTER_UPPERCASE, LEXEME::LETTER_LOWERCASE, LEXEME::DIGIT), TokenType::KEY };
				case LEXEME::EQUALS: // Setter
					return Token{ ch, TokenType::SETTER };
				case LEXEME::NEWLINE: // Newline
					return Token{ ch, TokenType::NEWLINE };
				case LEXEME::_EOF: // End of File
					return Token{ ch, TokenType::END };
				default: // any other character is null
					return Token{ ch, TokenType::NULL_TYPE };
				}
			}
		};
		// Parses the token container returned from ConfigTokenizer
		class ConfigParser : public TokenParserBase {
		public:
			ConfigParser(ConfigTokenizer&& tkiz) : TokenParserBase(std::forward<ConfigTokenizer>(tkiz), 256ull) {}

			operator std::unordered_map<std::string, RGB<short>>() const
			{
				std::unordered_map<std::string, RGB<short>> map;
				std::string key;
				std::string val;
				bool setter{ false };

				const auto insert{ [&map, &key, &val, &setter]() {
					if (!key.empty() && setter && !val.empty())
						map.insert_or_assign(key, hex_to_rgb(val));
					key = {};
					setter = false;
					val = {};
				} };

				for (auto& t : tokens) {
					switch (t.second) {
					case TokenType::KEY:
						key = t.first;
						break;
					case TokenType::SETTER:
						setter = true;
						break;
					case TokenType::NUMBER_HEX: [[fallthrough]];
					case TokenType::NUMBER:
						val = t.first;
						break;
					case TokenType::NEWLINE: [[fallthrough]];
					case TokenType::END:
						insert();
						break;
					default: // any other token type
						break;
					}
				}

				return map;
			}
		};

		inline std::unordered_map<std::string, RGB<short>> read_config(const std::string& filename)
		{
			return ConfigParser(std::move(ConfigTokenizer{ file::read(filename) })).operator std::unordered_map<std::string, RGB<short>>();
		}
	}

	struct ConfigDefaults {
		using sRGB = RGB<short>;
		using Map = std::unordered_map<std::string, sRGB>;

		Map _map;

		ConfigDefaults(Map&& defaults) : _map{ std::move(defaults) } {}

		operator Map() const { return _map; }
	};

	class Config {
	public:
		using sRGB = RGB<short>;
		using Map = std::unordered_map<std::string, sRGB>;

	protected:
		Map _map;
		bool _enabled{ true };

		template<size_t i = 0ull, typename... Ts>
		setcolor set(const std::tuple<Ts...>& tpl) const
		{
			if (_enabled) {
				if (const auto here{ std::get<i>(tpl) }; _map.contains(here)) {
					if (auto ret{ _map.at(here) }; ret != setcolor_placeholder)
						return ret;
				}
				if constexpr (i + 1ull < sizeof...(Ts))
					return set<i + 1ull>(tpl);
			}
			return setcolor_placeholder;
		}
	public:
		Config() = default;
		Config(Map&& map) : _map{ std::move(map) } {}
		Config(const std::string& filename) : _map{ fileext::read_config(filename) } {}
		Config(const palette<std::string>& p) : _map{ [&p]() {
			Map map;
			for (const auto& [key, sgr] : p.operator std::unordered_map<std::string, color::setcolor, std::hash<std::string>, std::equal_to<std::string>, std::allocator<std::pair<const std::string, color::setcolor>>>())
				map.insert_or_assign(key, sgr_to_rgb<short>(sgr));
			return std::move(map); // who needs copy ellision? not this lambda!					DISCLAIMER: this lambda may need copy ellision
		}() }{}
		template<typename KeyType>
		Config(const palette<KeyType>& p, const std::function<std::string(KeyType)>& conv) : _map{
			[](){
				Map map;
				for (const auto& [key, sgr] : p.operator palette<KeyType>::PaletteType())
					map.insert_or_assign(conv(key), sgr_to_rgb<short>(sgr));
				return std::move(map); // who needs copy ellision? not this lambda either!		DISCLAIMER: this lambda may need copy ellision
		}()} {}

		/**
		 * @brief			Enable or disable this instance's set functions from actually returning anything.
		 * @param enabled	When true, this instance is enabled and will emit escape sequences.
		 * @returns			bool
		 */
		bool setEnabled(const bool& enabled)
		{
			const auto copy{ _enabled };
			_enabled = enabled;
			return copy;
		}

		/**
		 * @brief		Retrieve a setcolor instance that will set the terminal color to the given key's value.
		 * @param key	The name of the target key.
		 * @returns		setcolor
		 */
		setcolor set(const std::string& key) const
		{
			if (_enabled && _map.contains(key))
				return{ _map.at(key) };
			return setcolor_placeholder;
		}
		/**
		 * @brief			Retrieve a setcolor instance that will set the terminal color to the given key's value.
		 * @param key...	The names of the target keys. The first existing keyname will be used.
		 * @returns			setcolor
		 */
		template<typename... Ts>
		setcolor set(const Ts&... keys) const
		{
			if (_enabled)
				return set<0ull>(std::make_tuple(keys...));
			return setcolor_placeholder;
		}

		setcolor reset_f() const
		{
			if (_enabled)
				return color::reset_f;
			return setcolor_placeholder;
		}

		setcolor reset_b() const
		{
			if (_enabled)
				return color::reset_b;
			return setcolor_placeholder;
		}

		/**
		 * @brief	Reset the terminal colors if this instance is enabled.
		 * @returns	setcolor
		 */
		setcolor reset() const
		{
			if (_enabled)
				return color::reset;
			return setcolor_placeholder;
		}

		/**
		 * @brief		Reset the terminal colors to a new key's value.
		 * @param key	Target keyname.
		 * @returns		setcolor
		 */
		setcolor reset(const std::string& key) const
		{
			if (_enabled)
				return reset() + set(key);
			return setcolor_placeholder;
		}
		/**
		 * @brief			Reset the terminal colors to a new key's value.
		 * @param keys...	Target keyname.
		 * @returns			setcolor
		 */
		template<typename... Ts>
		setcolor reset(const Ts&... keys) const
		{
			if (_enabled)
				return reset() + set<0ull>(std::make_tuple(keys...));
			return setcolor_placeholder;
		}

		friend std::ostream& operator<<(std::ostream& os, const Config& cfg)
		{
			for (auto& [key, val] : cfg._map)
				os << key << " = #" << color::rgb_to_hex(val) << '\n';
			return os;
		}

		/**
		 * @brief	Convert this color config instance to a color palette using std::string as the key type.
		 * @returns	palette<std::string>
		 */
		palette<std::string> to_palette() const
		{
			palette<std::string>::PaletteType p;
			p.reserve(_map.size());
			for (const auto& [key, color] : _map)
				p.insert_or_assign(key, rgb_to_sgr(color));
			return palette<std::string>{ std::move(p) };
		}

		/**
		 * @brief				Convert this color config instance to a color palette using a custom key type.
		 * @tparam KeyType		The palette's desired key type.
		 * @param conv			Function to convert from std::string to KeyType.
		 * @returns				palette<KeyType>
		 */
		template<typename KeyType>
		palette<KeyType> to_palette(const std::function<KeyType(std::string)>& conv) const
		{
			using PaletteType = palette<KeyType>::PaletteType;
			PaletteType p;
			p.reserve(_map.size());
			for (const auto& [key, color] : _map)
				p.insert_or_assign(conv(key), rgb_to_sgr(color));
			return palette<KeyType>{ std::move(p) };
		}
	};
}