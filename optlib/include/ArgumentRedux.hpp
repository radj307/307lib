#pragma once
#include <sysarch.h>
#include <TokenRedux.hpp>

#include <string>
#include <ostream>

namespace opt {
	namespace tkn {
		#pragma region DEFINITIONS_PACKAGE
		using namespace ::token;
		enum class LEXEME : char {
			NULL_TYPE,
			WHITESPACE,
			DASH,
			SLASH,
			BACKSLASH,
			EQUALS,
			COLON,
			QUOTE_SINGLE,
			QUOTE_DOUBLE,
			DIGIT,
			PERIOD,
			COMMA,
			LETTER,
			CHARACTER,
		};

		struct LexemeDictionary final : public base::LexemeDictBase<LEXEME> {
			/**
			 * @brief		Retrieve the lexeme associated with the given character.
			 * @param ch	Input Character
			 * @returnS		LexemeT
			 */
			LexemeT char_to_lexeme(const char& ch) const noexcept override
			{
				if (isdigit(ch))
					return LEXEME::DIGIT;
				else if (isalpha(ch))
					return LEXEME::LETTER;
				else if (isspace(ch))
					return LEXEME::WHITESPACE;
				switch (ch) {
				case '.':		return LEXEME::PERIOD;
				case ',':		return LEXEME::COMMA;
				case ':':		return LEXEME::COLON;
				case '=':		return LEXEME::EQUALS;
				case '\'':		return LEXEME::QUOTE_SINGLE;
				case '\"':		return LEXEME::QUOTE_DOUBLE;
				case '-':		return LEXEME::DASH;
				case '/':		return LEXEME::SLASH;
				case '\\':		return LEXEME::BACKSLASH;
				default:		return LEXEME::CHARACTER;
				}
			}
		};

		enum class Type {
			NULL_TYPE,
			_EOF,
			PARAMETER,
			OPTION,
			FLAG,
			SETTER,
			STRING,
		};

		using Token = base::TokenBase<Type>;
		#pragma endregion DEFINITIONS_PACKAGE
		#pragma region FORMAT_PACKAGE

		class Tokenizer : public base::TokenizerBase<LEXEME, LexemeDictionary, Type, Token> {
			bool _allowSlash;

		public:
			Tokenizer(std::stringstream&& ss, const bool& allow_slash_prefix = false) : TokenizerBase<LEXEME, LexemeDictionary, Type, Token>(std::forward<std::stringstream>(ss), LEXEME::WHITESPACE), _allowSlash{ allow_slash_prefix } {}

			/**
			 * @brief			Enable of disable using the slash character as an additional type prefix to dashes.
			 * @param enable	When true, slashes can be prefixes.
			 * @returns			bool
			 *\n				Previous value.
			 */
			bool setSlashEnabled(const bool& enable)
			{
				const auto copy{ _allowSlash };
				_allowSlash = enable;
				return copy;
			}
			/**
			 * @brief	Check if slashes may be considered additional type prefixes to dashes.
			 * @returns	bool
			 *\n		Current value.
			 */
			bool getSlashEnabled() const { return _allowSlash; }

			Token getNext() override
			{
				const char ch{ getch() };
				switch (get_lexeme(ch)) {
				case LEXEME::SLASH:
					if (_allowSlash) [[fallthrough]];
					else			 goto HANDLE_PARAMETER; //////////> GOTO
				case LEXEME::DASH:
				{
					auto next{ peeklex(LEXEME::NULL_TYPE) };
					if (next == LEXEME::NULL_TYPE)
						return{ Type::_EOF };

					if (next == LEXEME::DASH) { // is option
						eatnext(); // eat the dash
						return{ Type::OPTION, getnotsimilar(LEXEME::WHITESPACE) };
					}

					else if (next == LEXEME::DIGIT) // negative number
						goto HANDLE_PARAMETER; //////////> GOTO

					// else is a flag
					else return{ Type::FLAG, getnotsimilar(LEXEME::WHITESPACE, LEXEME::EQUALS) };
				}
				case LEXEME::DIGIT: [[fallthrough]];
				case LEXEME::LETTER:
				HANDLE_PARAMETER: /////////< GOTO TARGET
					return{ Type::PARAMETER, getnotsimilar(LEXEME::WHITESPACE) };
				case LEXEME::EQUALS:
					return{ Type::SETTER, ch };
				case LEXEME::QUOTE_DOUBLE:
					return{ Type::STRING, getuntil_unescaped('\"') };
				case LEXEME::QUOTE_SINGLE:
					return{ Type::STRING, getuntil_unescaped('\'') };
				default:
					return Token{ Type::NULL_TYPE, ch };
				}
			}
		};

		#pragma endregion FORMAT_PACKAGE
	}
	namespace types {
		class ArgumentBase {
		protected:
			std::string _value;

		public:
			ArgumentBase(const std::string& s) : _value{ s } {}

			friend std::ostream& operator<<(std::ostream& os, const ArgumentBase& arg)
			{
				return os;
			}
		};
	}
}
