/**
 * @file	TokenReduxDefaultDefs.hpp
 * @author	radj307
 * @brief	Provides the legacy tokenization types for use with the new API, as well as an example of implementing a definitions package.
 */
#pragma once
#include <sysarch.h>
#include <TokenRedux.hpp>

namespace token::DefaultDefs {
	/**
	 * @enum LEXEME
	 * @brief Represents the partially-abstracted type of a character. This acts as an intermediary between the raw character types and a Token Type, and LEXEMEs are not recognized by anything downstream from the _TokenizerBase_ class & derivatives, including the _Token_ struct.
	 *\n	  A lexeme is the smallest unit in lexical analysis, and represents a character or sequence of characters.
	 */
	enum class LEXEME : char {
		NULL_TYPE,						///< @brief Any unrecognized, invalid character.
		UNKNOWN,						///< @brief An unrecognized character that is not invalid.
		ESCAPE,							///< @brief An escape character, usually a backslash.
		EQUALS,							///< @brief '='
		ADDITION,						///< @brief '+'
		SUBTRACT,						///< @brief '-'
		UNDERSCORE,						///< @brief '_'
		COLON,							///< @brief ':'
		SEMICOLON,						///< @brief ';'
		POUND,							///< @brief '#'
		PERCENT,						///< @brief '%'
		QUOTE_SINGLE,					///< @brief '''
		QUOTE_DOUBLE,					///< @brief '"'
		BRACKET_OPEN,					///< @brief '{'
		BRACKET_CLOSE,					///< @brief '}'
		PARENTHESIS_OPEN,				///< @brief '('
		PARENTHESIS_CLOSE,				///< @brief ')'
		SQUAREBRACKET_OPEN,				///< @brief '['
		SQUAREBRACKET_CLOSE,			///< @brief ']'
		ANGLEBRACKET_OPEN,				///< @brief '<'
		ANGLEBRACKET_CLOSE,				///< @brief '>'
		LETTER_UPPERCASE,				///< @brief "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		LETTER_LOWERCASE,				///< @brief "abcdefghijklmnopqrstuvwxyz"
		DIGIT,							///< @brief "0123456789"
		PERIOD,							///< @brief '.'
		COMMA,							///< @brief ','
		WHITESPACE,						///< @brief " \t\v\r"
		NEWLINE,						///< @brief '\n'
		_EOF,							///< @brief Character with the value -1, or an imaginary end-of-file character.
	};

	/// @brief	Lexeme Dictionary
	struct LexemeDict : public base::LexemeDictBase<LEXEME> {
		LEXEME char_to_lexeme(char const&) const noexcept override;
	};

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
		HEADER,			///< @brief Header type, used by some file formats (INI "[]") similarly to a key, but with special syntax.
		SETTER,			///< @brief Setter type, used by some file formats (INI '=', JSON ':') to indicate a variable being defined.
		OPEN_SECTION,	///< @brief Section open type, used by JSON & similar formats. Must have a matching CLOSE_SECTION.
		CLOSE_SECTION,	///< @brief Section close type, used by JSON & similar formats. Must have a matching OPEN_SECTION.
		OPEN_ARRAY,		///< @brief Array open type, used by JSON & similar formats. Must have a matching CLOSE_ARRAY.
		CLOSE_ARRAY,	///< @brief Array close type, used by JSON & similar formats. Must have a matching OPEN_ARRAY.
	};

	/// @brief	Represents a section of data with a similar type.
	using Token = base::TokenBase<TokenType>;
}