/**
 * @file INIParser.hpp
 * @author radj307
 * @brief Contains the INIParser struct for parsing INIContainer objects.
 */
#pragma once
#include <TokenizedContainer.hpp>
#include <TokenizerINI.hpp>
#ifdef USE_DEPRECATED_INI
#include <ContainerINI.hpp> // legacy
#endif
#include <INIContainer.hpp> // new
#include <fileio.hpp>
#include <variant>

namespace token::parse {
	/**
	 * @struct INIParser
	 * @brief Parse a tokenized INI file into an INIContainer::Map (implicit), or a legacy container type (explicit).
	 */
	struct INIParser final : public TokenizedContainer {
	private:
		const std::string& filename;

		void throwEx(const size_t line, const std::string& msg)
		{
			throw std::exception(str::stringify("Syntax Error: ", msg, " at line ", line, " in file: ", filename).c_str());
		}

	public:
		/**
		 * @brief Stream Constructor
		 * @param file	- rvalue reference to a std::stringstream containing the contents of an INI-formatted file.
		 */
		INIParser(const std::string& filename, std::stringstream&& file) : TokenizedContainer(std::move(TokenizerINI(std::move(file)).tokenize())), filename{ filename } {}
		/**
		 * @brief Filename Constructor
		 * @param filename	- The name/path to an INI file.
		 */
		INIParser(const std::string& filename) : INIParser(filename, std::move(file::read(filename))) {}

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

			using enum TokenType;
			strip_types(COMMENT); // remove all comments

			/// @brief Lambda for throwing exceptions with consistent naming conventions.
			//const auto throwEx{ [&ln](const std::optional<std::string>& msg = std::nullopt) {
			//	throw std::exception(str::stringify("Invalid INI: ", msg.value_or("Syntax Error"), " at Line: ", ln).c_str());
			//} };
			/// @brief Lambda that handles variable verification & insertion into the map.
			const auto insert{ [this, &ln, &map, &header, &key, &value, &setter]() {
				if (!key.has_value())
					throwEx(ln, "Missing Key Declaration");
				if (!value.has_value())
					throwEx(ln, "Missing Value Declaration");
				map[header].insert_or_assign(key.value(), value.value());
			} };
			/// @brief Lambda that returns true if any temp variable is defined. This is used to detect whether a NEWLINE occurrance is valid or not
			const auto any_defined{ [&key, &value, &setter]() { return key.has_value() || value.has_value() || setter; } };

			size_t i{ 0ull };
			for (auto& [str, type] : tokens) {
				++i; // increment index by one
				switch (type) {
				case HEADER: // set the header
					header = str;
					break;
				case SETTER: // set the temp setter variable to true if it isn't already
					if (!setter)
						setter = true;
					else throwEx(ln, "Duplicate Setters"); // throw duplicate exception if setter was already true
					break;
				case KEY: // set the temp key
					if (!key.has_value()) {
						key = str;
						break;
					}
					else [[fallthrough]]; // if the key is already set, parse as a string value
				case STRING: // set the temp value to a string
					if (!setter)
						throwEx(ln, "Missing Setter");
					if (value.has_value() && std::holds_alternative<String>(value.value()))
						value = (std::get<String>(value.value()) + ' ' + str); // allow fallthrough keys to be appended
					else
						value = str;
					break;
				case NUMBER: // set the temp value to a long double
					if (!setter)
						throwEx(ln, "Missing Setter");
					value = str::stold(str);
					break;
				case NUMBER_INT: // set the temp value to an integer
					if (!setter)
						throwEx(ln, "Missing Setter");
					value = str::stoll(str);
					break;
				case BOOLEAN: { // set the temp value to a boolean
					if (!setter)
						throwEx(ln, "Missing Setter");
					auto b{ str::string_to_bool(str) };
					if (!b.has_value())
						throwEx(ln, "Invalid Boolean");
					value = std::move(b);
					break;
				}
				case NEWLINE: // insert & reset temp variables
					if (any_defined())
						insert();
					key = std::nullopt;
					value = std::nullopt;
					setter = false;
					++ln;
					break;
				case END: // insert temp variables and return successfully.
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

	#ifdef USE_DEPRECATED_INI
		/// @brief Legacy INI container type
		explicit operator file::ini::ContainerINI::SectionMap()
		{
			using Optstr = std::optional<std::string>;

			Optstr header, key, value;
			bool setter{ false };
			size_t ln{ 1ull };

			file::ini::ContainerINI::SectionMap map{};

			using enum TokenType;
			strip_types(COMMENT);

			const auto throwEx{ [&ln](const std::optional<std::string>& msg = std::nullopt) {
				throw std::exception(str::stringify("Invalid INI: ", msg.value_or("Syntax Error"), " at Line: ", ln).c_str());
			} };

			const auto insert{ [&map, &header, &key, &value, &setter, &throwEx]() {
				if (!header.has_value())
					throwEx("Missing Header Declaration");
				if (!key.has_value())
					throwEx("Missing Key Declaration");
				if (!value.has_value())
					throwEx("Missing Value Declaration");
				map[header.value()].insert_or_assign(key.value(), value.value());
			} };

			for (auto& [str, type] : tokens) {
				switch (type) {
				case HEADER:
					header = str;
					break;
				case KEY:
					key = str;
					break;
				case SETTER:
					if (!setter)
						setter = true;
					else throwEx("Duplicate Setters");
					break;
				case STRING: [[fallthrough]];
				case NUMBER: [[fallthrough]];
				case BOOLEAN:
					value = str;
					break;
				case NEWLINE:
					if (setter)
						insert();
					key = std::nullopt;
					value = std::nullopt;
					setter = false;
					++ln;
					break;
				case END:
					if (setter)
						insert();
					return map;
				}
			}
			throwEx("Missing Token: END / EOF");
			return{}; // this will never be reached, but it prevents compiler warning
		}
	#endif
	};
}