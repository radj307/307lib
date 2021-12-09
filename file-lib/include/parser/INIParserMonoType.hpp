/**
 * @file INIParserMonoType.hpp
 * @author radj307
 * @brief Contains the INIParserMonoType struct.
 */
#pragma once
#include <TokenizedContainer.hpp>
#include <TokenizerINI.hpp>
#include <INIContainerMonoType.hpp> // new
#include <fileio.hpp>
#include <variant>

namespace token::parse {
	template<ValidValueT T>
	T convert_to(const std::string& str)
	{
		if constexpr (std::same_as<T, Boolean>)
			return str::string_to_bool(str).value_or(false);
		else if constexpr (std::same_as<T, String>)
			return str;
		else if constexpr (std::same_as<T, Float>)
			return str::stold(str);
		else if constexpr (std::same_as<T, Integer>)
			return str::stoll(str);
		return static_cast<T>(0);
	}

	/**
	 * @struct INIParserMonoType
	 * @brief Parse a tokenized INI file into an INIContainer::Map (implicit), or a legacy container type (explicit).
	 */
	template<ValidValueT T>
	struct INIParserMonoType final : public TokenizedContainer {
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
		INIParserMonoType(const std::string& filename, std::stringstream&& file) : TokenizedContainer(std::move(TokenizerINI(std::move(file), true).tokenize())), filename{ filename } {}
		/**
		 * @brief Filename Constructor
		 * @param filename	- The name/path to an INI file.
		 */
		INIParserMonoType(const std::string& filename) : INIParserMonoType(filename, std::move(file::read(filename))) {}

		/// @brief Revised INI container type
		operator INIContainerMonoType<T>::Map()
		{
			// Init temporary variables:
			std::string header{ "" }; // ini standard allows empty headers
			std::optional<std::string> key, value;
			bool setter{ false };
			size_t ln{ 1ull };
			// Init map:
			typename INIContainerMonoType<T>::Map map{};

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
				map[header].insert_or_assign(key.value(), convert_to<T>(value.value()));
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
				case NUMBER: [[fallthrough]];
				case NUMBER_INT: [[fallthrough]];
				case BOOLEAN: [[fallthrough]];
				case STRING: // set the temp value to a string
					if (!setter)
						throwEx(ln, "Missing Setter");
					if (value.has_value())
						value = (std::string(value.value()) + " "s + str); // allow fallthrough keys to be appended
					else
						value = str;
					break;
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
	};
}