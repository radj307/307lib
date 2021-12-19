/**
 * @file Token.hpp
 * @author radj307
 * @brief Contains enums __LEXEME__ & __TokenType__, as well as the Token struct used by the TokenizerBase class and derivatives.
 */
#pragma once
#include <LEXEME.h>

#include <str.hpp>
#include <var.hpp>

#include <string>
#include <utility>

 /**
  * @namespace token
  * @brief Contains objects, methods, types, and functions related to tokenizing files.
  */
namespace token {
	/**
	 * @enum TokenType
	 * @brief This enum provides generic types that apply to some/all file formats. If a file format does not use one of the types listed here, it is safe to ignore it as types should never be required by any function.
	 */
	enum class TokenType : unsigned short {
		NULL_TYPE,		///< @brief Null type, indicates that a character is invalid or an error occurred.
		UNKNOWN,		///< @brief Unknown type, this should be the default return value of functions derived from TokenizerBase::getNext()
		ESCAPED,		///< @brief Escaped character, may appear inside of other tokens.
		NEWLINE,		///< @brief Newline / line feed
		END,			///< @brief End of file
		DIVIDER,		///< @brief Divider used by some languages (JSON ',') in lists to separate elements.
		COMMENT,		///< @brief Comment type, indicates a comment that can be ignored or used should that be needed.
		STRING,			///< @brief String type
		KEY,			///< @brief Key type, used by some file formats (INI) that accept string values without enclosing quotes.
		BOOLEAN,		///< @brief Boolean type, should only ever match "true" or "false" (implementation-defined-case-sensitivity)
		NUMBER,			///< @brief Number type.
		NUMBER_INT,		///< @brief Integer type, used by some file formats (INI) to differentiate between floating-points and integrals.
		NUMBER_HEX,		///< @brief Hexadecimal number type, used by some file formats.
		HEADER,			///< @brief Header type, used by some file formats (INI "[]") similarly to a key, but with special syntax.
		SETTER,			///< @brief Setter type, used by some file formats (INI '=', JSON ':') to indicate a variable being defined.
		OPEN_SECTION,	///< @brief Section open type, used by JSON & similar formats. Must have a matching CLOSE_SECTION.
		CLOSE_SECTION,	///< @brief Section close type, used by JSON & similar formats. Must have a matching OPEN_SECTION.
		OPEN_ARRAY,		///< @brief Array open type, used by JSON & similar formats. Must have a matching CLOSE_ARRAY.
		CLOSE_ARRAY,	///< @brief Array close type, used by JSON & similar formats. Must have a matching OPEN_ARRAY.
	};

	/**
	 * @brief Convert a TokenType to a user-friendly string.
	 * @param type	- Input type
	 * @returns std::string
	 */
	inline std::string get_tokentype_plaintext(const TokenType& type)
	{
		switch (type) {
		case TokenType::NULL_TYPE:
			return "NULL";
		case TokenType::NEWLINE:
			return "New Line";
		case TokenType::END:
			return "End of File";
		case TokenType::DIVIDER:
			return "Divider";
		case TokenType::COMMENT:
			return "Comment";
		case TokenType::STRING:
			return "String";
		case TokenType::KEY:
			return "Key";
		case TokenType::BOOLEAN:
			return "Boolean";
		case TokenType::NUMBER:
			return "Number";
		case TokenType::NUMBER_INT:
			return "Integer";
		case TokenType::NUMBER_HEX:
			return "Hexadecimal";
		case TokenType::HEADER:
			return "Header";
		case TokenType::SETTER:
			return "Setter";
		case TokenType::ESCAPED:
			return "Escaped Character";
		case TokenType::OPEN_ARRAY:
			return "Open Array";
		case TokenType::CLOSE_ARRAY:
			return "Close Array";
		case TokenType::OPEN_SECTION:
			return "Open Section";
		case TokenType::CLOSE_SECTION:
			return "Close Section";
		case TokenType::UNKNOWN: [[fallthrough]];
		default:
			return "Unknown";
		}
	}

	using AccessibleToken = std::pair<std::string, TokenType>;

	template<typename TokenT>
	struct TokenBase {
	protected:
		std::string _str;
		TokenT _type;
	public:
		/**
		 * @brief String value constructor
		 * @param str	- String of characters in the file that all have the same type, and would be part of the same primary "element" in a given file format.
		 * @param type	- This token's type.
		 */
		TokenBase(std::string str, TokenT type) : _str{ std::move(str) }, _type{ std::move(type) } {}
		/**
		 * @brief Char value constructor
		 * @param ch	- Single character in the file that has a single type. This is converted to a std::string during initialization.
		 * @param type	- This token's type.
		 */
		TokenBase(char ch, TokenT type) : _str{ std::move(std::string(1u, std::move(ch))) }, _type{ std::move(type) } {}
		/**
		 * @brief Null value constructor. Results in an empty _str value.
		 * @param type	- This token's type.
		 */
		TokenBase(TokenT type) : _type{ std::move(type) } {}
		/**
		 * @brief Pair value constructor
		 * @param pr	- String of characters in the file that all have the same type, and would be part of the same primary "element" in a given file format, and the token's type.
		 */
		TokenBase(std::pair<std::string, TokenT> pr) : TokenBase(std::move(pr.first), std::move(pr.second)) {}
		/**
		 * @brief Pair value constructor
		 * @param pr	- Single character in the file that has a single type. This is converted to a std::string during initialization, and this token's type.
		 */
		TokenBase(std::pair<char, TokenT> pr) : TokenBase(std::move(pr.first), std::move(pr.second)) {}
	};

	/**
	 * @struct Token
	 * @brief Base unit used by the Tokenizer as an intermediary between raw characters, and the abstract elements of a file format, as well as containing the actual string in the parsed data that it represents.
	 */
	struct Token : TokenBase<TokenType> {
		/**
		 * @brief String value constructor
		 * @param str	- String of characters in the file that all have the same type, and would be part of the same primary "element" in a given file format.
		 * @param type	- This token's type.
		 */
		Token(std::string str, TokenType type) : TokenBase(std::move(str), std::move(type)) {}
		/**
		 * @brief Char value constructor
		 * @param ch	- Single character in the file that has a single type. This is converted to a std::string during initialization.
		 * @param type	- This token's type.
		 */
		Token(char ch, TokenType type) : TokenBase(std::move(std::string(1u, std::move(ch))), std::move(type)) {}
		/**
		 * @brief Null value constructor. Results in an empty _str value.
		 * @param type	- This token's type.
		 */
		Token(TokenType type) : TokenBase(std::move(type)) {}
		/**
		 * @brief Pair value constructor
		 * @param pr	- String of characters in the file that all have the same type, and would be part of the same primary "element" in a given file format, and the token's type.
		 */
		Token(std::pair<std::string, TokenType> pr) : Token(std::move(pr.first), std::move(pr.second)) {}
		/**
		 * @brief Pair value constructor
		 * @param pr	- Single character in the file that has a single type. This is converted to a std::string during initialization, and this token's type.
		 */
		Token(std::pair<char, TokenType> pr) : Token(std::move(pr.first), std::move(pr.second)) {}

		/// @brief String operator. Returns the string value contained.
		operator const std::string() const { return _str; }
		operator const TokenType() const { return _type; }
		/// @brief Pair conversion operator.
		operator AccessibleToken() const { return{ str(), type() }; }
		Token& operator=(const Token& o) { _str = o._str; _type = o._type; return *this; }

		friend std::ostream& operator<<(std::ostream& os, const Token& token)
		{
			if (token._type != TokenType::NEWLINE) // prevent unintended newlines in stdout
				os << token.operator const std::string();
			else os << "\\n";
			return os;
		}

		bool operator==(const Token& o) const { return _str == o._str && _type == o._type; }
		bool operator!=(auto&& o) const { return !operator==(std::forward<decltype(o)>(o)); }

		TokenType type() const { return _type; }
		std::string str(const bool allow_special_characters = false) const
		{
			if (allow_special_characters)
				return this->operator const std::string();
			auto str{ _str };
			for (auto esc{ str.find(ESCAPE_CHARACTER) }; str::pos_valid(esc); esc = str.find(ESCAPE_CHARACTER, esc + 2u))
				str = str.substr(0u, esc) + "\\" + str.substr(esc);
			return str;
		}

		bool is_type(const TokenType& type) const { return _type == type; }
		bool is_null() const { return is_type(TokenType::NULL_TYPE); }
		bool is_unknown() const { return is_type(TokenType::UNKNOWN); }
		bool is_valid() const { return !is_null() && !is_unknown(); }
		bool is_eof() const { return is_type(TokenType::END); }
		static bool is_null(const TokenType& type) { return type == TokenType::NULL_TYPE; }
		static bool is_unknown(const TokenType& type) { return type == TokenType::UNKNOWN; }
		static bool is_valid(const TokenType& type) { return !is_null(type) && !is_unknown(type); }
	};
}