#pragma once
#include <color-format.hpp>
#include <color-transform.hpp>
#include <fileio.hpp>
#include <fileutil.hpp>
#include <token/TokenizerBase.hpp>
#include <token/TokenizedContainer.hpp>
#include <setcolor.hpp>

#include <string>
#include <unordered_map>
#include <tuple>
#include <ostream>

namespace color {

	class Config;

	namespace fileext {
		using namespace token;
		class ConfigTokenizer : public TokenizerBase {
		public:
			ConfigTokenizer(std::stringstream&& buffer) : TokenizerBase(std::forward<std::stringstream>(buffer)) {}
			Token getNext() override
			{
				using enum LEXEME;
				const auto ch{ getch() };
				switch (get_lexeme(ch)) {
				case POUND: // Beginning of a hex color value
					return Token{ getsimilar(DIGIT, LETTER_UPPERCASE, LETTER_LOWERCASE), TokenType::NUMBER };
				case LETTER_UPPERCASE: [[fallthrough]];
				case LETTER_LOWERCASE: // Key name
					return Token{ std::string(1ull, ch) + getsimilar(LETTER_UPPERCASE, LETTER_LOWERCASE, DIGIT), TokenType::KEY };
				case EQUALS: // Setter
					return Token{ ch, TokenType::SETTER };
				case NEWLINE: // Newline
					return Token{ ch, TokenType::NEWLINE };
				case _EOF: // End of File
					return Token{ ch, TokenType::END };
				default:
					return Token{ ch, TokenType::NULL_TYPE };
				}
			}
		};

		class ConfigParser : public TokenizedContainer {
		public:
			ConfigParser(ConfigTokenizer&& tkiz) : TokenizedContainer(std::forward<ConfigTokenizer>(tkiz), 256ull) {}

			operator std::unordered_map<std::string, RGB<short>>() const
			{
				std::unordered_map<std::string, RGB<short>> map;
				std::string key;
				std::string val;
				bool setter{ false };

				const auto insert{ [&map, &key, &val, &setter]() {
					if (!key.empty() && setter && !val.empty()) {
						map.insert_or_assign(key, hex_to_rgb(val));
					}
				} };

				for (auto& t : tokens) {
					using enum TokenType;
					switch (t.second) {
					case KEY:
						key = t.first;
						break;
					case SETTER:
						setter = true;
						break;
					case NUMBER:
						val = t.first;
						break;
					case NEWLINE:
						insert();
						key = {};
						setter = false;
						val = {};
						break;
					case END:
						insert();
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
	class Config {
	public:
		using RGB = RGB<short>;
		using Map = std::unordered_map<std::string, RGB>;

	protected:
		Map _map;
		bool _enabled{ true };

		template<size_t i = 0ull, typename... vT>
		setcolor set(const std::tuple<vT...>& tpl) const
		{
			if (_enabled) {
				if (const auto here{ std::get<i>(tpl) }; _map.contains(here)) {
					if (auto ret{ _map.at(here) }; ret != setcolor_placeholder)
						return ret;
				}
				if constexpr (i + 1ull < sizeof...(vT))
					return set<i + 1ull>(tpl);
			}
			return setcolor_placeholder;
		}
	public:
		Config() = default;
		Config(Map&& map) : _map{ std::move(map) } {}
		Config(const std::string& filename) : _map{ fileext::read_config(filename) } {}

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
		template<typename... vT>
		setcolor set(const vT&... keys) const
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
		template<typename... vT>
		setcolor reset(const vT&... keys) const
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
	};
}