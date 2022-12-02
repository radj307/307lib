#pragma once
#include <TokenReduxDefaultDefs.hpp>

namespace file::ini::tokenizer {
	using namespace token::DefaultDefs;

	/**
	 * @struct	INITokenizer
	 * @brief	Tokenizes the contents of an INI config file.
	 */
	struct INITokenizer : token::base::TokenizerBase<LEXEME, LexemeDict, ::token::DefaultDefs::TokenType, Token> {
		INITokenizer(std::stringstream&& buffer) : TokenizerBase(std::move(buffer), LEXEME::WHITESPACE, LEXEME::_EOF) {}
	protected:
		TokenT getNext() override
		{
			const auto ch{ getch() };
			switch (get_lexeme(ch)) {
			case LEXEME::POUND: [[fallthrough]];
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
			case LEXEME::LETTER_UPPERCASE:
			{ // the case of unenclosed string variables is handled by the parser, not the tokenizer
				const auto pos{ rollback() };
				std::string str;
				if (const auto lc{ str::tolower(ch) }; lc == 't' && getline_and_match(4u, "true", str))
					return{ str, TokenType::BOOLEAN };
				else if (lc == 'f' && getline_and_match(5u, "false", str))
					return{ str, TokenType::BOOLEAN };
				else
					ss.seekg(pos - 1ll);
				// getnotsimilar to newlines, equals, pound/semicolon '#'/';' (comments), and EOF
				return Token{ str::strip_line(getnotsimilar(LEXEME::NEWLINE, LEXEME::EQUALS, LEXEME::POUND, LEXEME::SEMICOLON, LEXEME::_EOF), "#;"), TokenType::KEY };
			}
			case LEXEME::SUBTRACT: [[fallthrough]]; // number start
			case LEXEME::DIGIT:
			{
				rollback();
				const auto str{ getsimilar(LEXEME::SUBTRACT, LEXEME::DIGIT, LEXEME::PERIOD) };
				return (str::pos_valid(str.find('.') || (!str.empty() && (str.back() == 'f' || str.back() == 'F'))) ? Token{str, TokenType::NUMBER} : Token{str, TokenType::NUMBER_INT});
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
