/**
 * @file INIParser.hpp
 * @author radj307
 * @brief Contains the INIParser struct for parsing INIContainer objects.
 */
#pragma once
#include <sysarch.h>
#include <fileio.hpp>
#include <Token.hpp>
#include <TokenParserBase.hpp>
#include <INI_Tokenizer.hpp>
#include <INI_Container.hpp>

#include <make_exception.hpp>

#include <variant>
#if LANG_CPP >= 17
#include <filesystem>
#endif

namespace token::parse {
	inline static bool INIParser_AllowBlankValue{ true };

	/**
	 * @struct INIParser
	 * @brief Parse a tokenized INI file into an INIContainer::Map (implicit), or a legacy container type (explicit).
	 */
	struct INIParser final : public TokenParserBase {
	private:
		const std::string& filename;

		void throwEx(const size_t line, const std::string& msg)
		{
			throw make_exception("Syntax Error: ", msg, " at line ", line, " in file: ", filename);
		}

	public:
		/**
		 * @brief Stream Constructor
		 * @param file	- rvalue reference to a std::stringstream containing the contents of an INI-formatted file.
		 */
		INIParser(const std::string& filename, std::stringstream&& file) : TokenParserBase(std::move(TokenizerINI(std::forward<std::stringstream>(file)).tokenize())), filename{ filename } {}
		INIParser(const std::string& filename) : INIParser(filename, std::move(file::read(filename))) {}
	#if LANG_CPP >= 17
		INIParser(const std::filesystem::path& path, std::stringstream&& file) : TokenParserBase(std::move(TokenizerINI(std::forward<std::stringstream>(file)).tokenize())), filename{ path.generic_string() } {}
		/**
		 * @brief Filename Constructor
		 * @param filename	- The name/path to an INI file.
		 */
		INIParser(const std::filesystem::path& path) : INIParser(path, std::move(file::read(path))) {}
	#endif

		/// @brief Revised INI container type
		operator INIContainer::Map()
		{
			// Init temporary variables:
			std::string header{ "" }; // ini standard allows empty headers
			std::optional<std::string> key;
			std::optional<VariableT> value;
			bool setter{ false };
			size_t ln{ 1ull };
			// Init map:
			INIContainer::Map map{};

			strip_types(TokenType::COMMENT); // remove all comments

			/// @brief Lambda for throwing exceptions with consistent naming conventions.
			//const auto throwEx{ [&ln](const std::optional<std::string>& msg = std::nullopt) {
			//	throw std::exception(str::stringify("Invalid INI: ", msg.value_or("Syntax Error"), " at Line: ", ln).c_str());
			//} };
			/// @brief Lambda that handles variable verification & insertion into the map.
			const auto insert{ [this, &ln, &map, &header, &key, &value, &setter]() {
				if (!key.has_value())
					throwEx(ln, "Missing Key Declaration");
				if (!value.has_value()) {
					if (INIParser_AllowBlankValue)
						value = std::string{};
					else
						throwEx(ln, "Missing Value Declaration");
				}
				map[header].insert_or_assign(key.value(), value.value());
			} };
			/// @brief Lambda that returns true if any temp variable is defined. This is used to detect whether a NEWLINE occurrance is valid or not
			const auto any_defined{ [&key, &value, &setter]() { return key.has_value() || value.has_value() || setter; } };

			size_t i{ 0ull };
			for (auto& [str, type] : tokens) {
				++i; // increment index by one (used to add _EOL)
				switch (type) {
				case TokenType::HEADER: // set the header
					header = str;
					break;
				case TokenType::SETTER: // set the temp setter variable to true if it isn't already
					if (!setter)
						setter = true;
					else throwEx(ln, "Duplicate Setters"); // throw duplicate exception if setter was already true
					break;
				case TokenType::KEY: // set the temp key
					if (!key.has_value()) {
						key = str;
						break;
					}
					else [[fallthrough]]; // if the key is already set, parse as a string value
				case TokenType::STRING: // set the temp value to a string
					if (!setter)
						throwEx(ln, "Missing Setter");
					if (value.has_value()) {
						if (std::holds_alternative<String>(value.value()))
							value = (std::get<String>(value.value()) + str); // allow fallthrough keys to be appended
						else throwEx(ln, "Duplicate Value");
					}
					else
						value = str;
					break;
				case TokenType::NUMBER: // set the temp value to a long double
					if (!setter)
						throwEx(ln, "Missing Setter");
					if (size_t decimal_count{ 0ull }; std::all_of(str.begin(), str.end(), [&decimal_count](auto&& ch) { if (ch == '.') ++decimal_count; return isdigit(ch) && decimal_count == 1; }))
						value = str::stold(str);
					else value = str;
					break;
				case TokenType::NUMBER_INT: // set the temp value to an integer
					if (!setter)
						throwEx(ln, "Missing Setter");
					if (std::all_of(str.begin(), str.end(), isdigit))
						value = str::stoll(str);
					else value = str;
					break;
				case TokenType::NUMBER_HEX:
					if (!setter)
						throwEx(ln, "Missing Setter");
					if (value.has_value() && std::holds_alternative<String>(value.value()))
						value = (std::get<String>(value.value()) + ' ' + str);
					else value = str;
					break;
				case TokenType::BOOLEAN: { // set the temp value to a boolean
					if (!setter)
						throwEx(ln, "Missing Setter");
					auto b{ str::string_to_bool(str) };
					if (!b.has_value())
						value = str;
					value = std::move(b);
					break;
				}
				case TokenType::NEWLINE: // insert & reset temp variables
					if (any_defined())
						insert();
					key = std::nullopt;
					value = std::nullopt;
					setter = false;
					++ln;
					break;
				case TokenType::END: // insert temp variables and return successfully.
					if (any_defined())
						insert();
					if (i == tokens.size()) // if this EOF is the last element in the tokens vec.
						return map;
					break;
				default: // throw unexpected token exception
					throwEx(ln, "Unexpected token type \"" + get_tokentype_plaintext(type) + "\"");
				}
			}
			// throw missing EOF exception
			throwEx(ln, "Missing Token: END / EOF");
			return{}; // this will never be reached, but it prevents compiler warning
		}
	};
}