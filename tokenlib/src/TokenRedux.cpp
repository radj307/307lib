#include "../include/TokenRedux.hpp"

using namespace token;
using namespace token::base;

template<typename LexemeType> LexemeType LexemeDictBase<LexemeType>::operator()(const char& ch) const noexcept 
{
	return char_to_lexeme(ch);
}
