#pragma once
#include <TokenRedux.hpp>

#include <string>
#include <ostream>

namespace opt {
	namespace types {
		enum class LEXEME : char {
			NULL_TYPE,
			DASH,
			EQUALS,
			QUOTE_SINGLE,
			QUOTE_DOUBLE,
			DIGIT,
			LETTER,
		};

		struct LexemeDictionary : token::LexemeDict<LEXEME> {

		};

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
