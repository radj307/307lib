/**
 * @file TokenizerINI.hpp
 * @author radj307
 * @brief Contains the TokenizerINI struct, a __TokenizerBase__ specialization for the INI file format.
 */
#pragma once
#include <TokenizerBase.hpp>

#include <make_exception.hpp>

namespace token {
	/// @brief INI tokenizer
	struct TokenizerINI : public TokenizerBase {
		const bool allow_whitespace_in_keyname{ false };

		/// @brief Default Constructor. To insert data into the local stream, use operator>> from a valid std::istream instance.
		TokenizerINI() = default;
		/// @brief Move-constructor that accepts a std::stringstream. @param buffer - This stream is moved to the tokenizer and used as the local stream.
		TokenizerINI(std::stringstream&& buffer, const bool& allow_whitespace_in_keyname = false) : TokenizerBase(std::move(buffer)), allow_whitespace_in_keyname{ allow_whitespace_in_keyname } {}

		/**
		 * @brief Returns a Token containing part of an INI file.
		 * @returns Token
		 */
		Token getNext() override
		{
			const auto ch{ getch() };
			switch (get_lexeme(ch)) {
			case token::LEXEME::POUND:
				// Check Hex Number
				if (const auto next{ peek() }; ishexnum(next)) // distinguish between hex numbers & comments
					return Token{ getsimilar(ishexnum), TokenType::NUMBER_HEX };
				[[fallthrough]];
			case LEXEME::SEMICOLON:// Comment
				return Token{ std::string(1ull, ch) += getline('\n', false), TokenType::COMMENT };
			case LEXEME::EQUALS: // Setter Start
				return Token{ ch, TokenType::SETTER };
			case LEXEME::QUOTE_SINGLE: // String (single) start
				return Token{ getline('\''), TokenType::STRING };
			case LEXEME::QUOTE_DOUBLE: // String (double) start
				return Token{ getline('\"'), TokenType::STRING };
			case LEXEME::SQUAREBRACKET_OPEN:
				return Token{ getline(']'), TokenType::HEADER };
			case LEXEME::LETTER_LOWERCASE: [[fallthrough]]; // if text appears without an enclosing quote, parse it as a key
			case LEXEME::LETTER_UPPERCASE: { // the case of unenclosed string variables is handled by the parser, not the tokenizer
				const auto pos{ rollback() };
				std::string str;
				if (const auto lc{ str::tolower(ch) }; lc == 't' && getline_and_match(4u, "true", str))
					return{ str, TokenType::BOOLEAN };
				else if (lc == 'f' && getline_and_match(5u, "false", str))
					return{ str, TokenType::BOOLEAN };
				else
					ss.seekg(pos - 1ll);
				// getsimilar
				return allow_whitespace_in_keyname ? Token{ getsimilar(LEXEME::LETTER_LOWERCASE, LEXEME::LETTER_UPPERCASE, LEXEME::UNDERSCORE, LEXEME::DIGIT, LEXEME::WHITESPACE), TokenType::KEY } : Token{ getsimilar(LEXEME::LETTER_LOWERCASE, LEXEME::LETTER_UPPERCASE, LEXEME::UNDERSCORE, LEXEME::DIGIT), TokenType::KEY };
			}
			case LEXEME::SUBTRACT: [[fallthrough]]; // number start
			case LEXEME::DIGIT: {
				rollback();
				const auto str{ getsimilar(LEXEME::SUBTRACT, LEXEME::DIGIT, LEXEME::PERIOD) };
				return (str::pos_valid(str.find('.') || !str.empty() && (str.back() == 'f' || str.back() == 'F')) ? Token{ str, TokenType::NUMBER } : Token{ str, TokenType::NUMBER_INT });
			}
			case LEXEME::NEWLINE:
				return Token{ ch, TokenType::NEWLINE };
			case LEXEME::WHITESPACE:
				throw make_exception("TokenizerINI::getNext()\tReceived unexpected whitespace character as input!");
			case LEXEME::ESCAPE:
				return Token{ std::string(1u, ch) += getch(true), TokenType::ESCAPED };
			case LEXEME::_EOF:
				return Token{ TokenType::END };
			default:
				return Token{ ch, TokenType::NULL_TYPE };
			}
		}
	};
}