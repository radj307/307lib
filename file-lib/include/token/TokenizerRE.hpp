#pragma once
#include <TokenizerBase.hpp>
namespace token {
	class TokenizerRE : TokenizerBase {
	public:
		TokenizerRE(std::stringstream&& buffer) : TokenizerBase(std::move(buffer)) {}

		Token getNext() override
		{
			const auto ch{ getch() };
			using enum LEXEME;
			switch (get_lexeme(ch)) {
			case DIGIT: [[fallthrough]];
			case LETTER_LOWERCASE: [[fallthrough]];
			case LETTER_UPPERCASE:
				rollback();
				return Token{ str::strip_line(getsimilar(LETTER_LOWERCASE, LETTER_UPPERCASE, DIGIT, WHITESPACE)), TokenType::STRING };
			case EQUALS:
				return Token{ ch, TokenType::SETTER };
			case BRACKET_OPEN:
				return Token{ ch, TokenType::OPEN_SECTION };
			case BRACKET_CLOSE:
				return Token{ ch, TokenType::CLOSE_SECTION };
			case SQUAREBRACKET_OPEN:
				return Token{ ch, TokenType::OPEN_ARRAY };
			case SQUAREBRACKET_CLOSE:
				return Token{ ch, TokenType::CLOSE_ARRAY };
			case NEWLINE:
				return Token{ ch, TokenType::NEWLINE };
			case _EOF:
				return Token{ TokenType::END };
			default:
				return Token{ ch, TokenType::NULL_TYPE };
			}
		}
	};
}