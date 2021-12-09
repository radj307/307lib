#pragma once
#include <TokenizerBase.hpp>

namespace token {
	struct TokenizerJSON : public TokenizerBase {
		TokenizerJSON() = default;
		TokenizerJSON(std::stringstream&& buffer) : TokenizerBase(std::move(buffer)) {}

		Token getNext() override
		{
			const auto ch{ getch() };
			using enum LEXEME;
			switch (get_lexeme(ch)) {
			case COMMA:
				return Token{ ch, TokenType::DIVIDER };
			case BRACKET_OPEN:
				return Token{ ch, TokenType::OPEN_SECTION };
			case BRACKET_CLOSE:
				return Token{ ch, TokenType::CLOSE_SECTION };
			case SQUAREBRACKET_OPEN:
				return Token{ ch, TokenType::OPEN_ARRAY };
			case SQUAREBRACKET_CLOSE:
				return Token{ ch, TokenType::CLOSE_ARRAY };
			case COLON:
				return Token{ ch, TokenType::SETTER };
			case QUOTE_DOUBLE:
				return Token{ getline('\"'), TokenType::STRING };
			case LETTER_LOWERCASE: [[fallthrough]];
			case LETTER_UPPERCASE: {
				rollback();
				std::string str;
				if (const auto lc{ str::tolower(ch) }; lc == 't' && getline_and_match(4u, "true", str))
					return Token{ str, TokenType::BOOLEAN };
				else if (lc == 'f' && getline_and_match(5u, "false", str))
					return Token{ str, TokenType::BOOLEAN };
				else
					throw std::exception(str::stringify("TokenizerJSON::getNext()\tInvalid Boolean Value: \"", str, '\"!').c_str());
				break;
			}
			case SUBTRACT: [[fallthrough]];
			case DIGIT:
				rollback();
				return Token{ getsimilar(SUBTRACT, DIGIT, PERIOD), TokenType::NUMBER };
			case NEWLINE:
				return Token{ ch, TokenType::NEWLINE };
			case ESCAPE:
				return Token{ std::string(1u, ch) += getch(true), TokenType::ESCAPED };
			case _EOF:
				return Token{ TokenType::END };
			default:
				return Token{ ch, TokenType::NULL_TYPE };
			}
		}
	};
}