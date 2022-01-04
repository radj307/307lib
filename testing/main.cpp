#include <TermAPI.hpp>

#include <fileio.hpp>
#include <TokenRedux.hpp>
#include <TokenReduxDefaultDefs.hpp>

using namespace file::DefaultDefs;

class Tkiz : public file::base::TokenizerBase<LEXEME, LexemeDict, file::DefaultDefs::TokenType, Token> {
public:
	Tkiz(std::stringstream&& ss) : TokenizerBase(std::forward<std::stringstream>(ss), file::DefaultDefs::LEXEME::WHITESPACE) {}

	file::DefaultDefs::Token getNext() override {
		using namespace file::DefaultDefs;
		auto ch{ getch() };
		switch (get_lexeme(ch)) {
		case LEXEME::NEWLINE:
			return Token{ TokenType::NEWLINE, ch };
		default:
			return Token{ TokenType::UNKNOWN };
		}
	}
};
class TkParse : public file::base::TokenParserBase<std::vector<Token>, Token> {
public:
	TkParse(std::vector<Token>&& vec) : TokenParserBase(std::forward<std::vector<Token>>(vec)) {}

	std::vector<Token> parse() const override {
		return tokens;
	}
};

int main(const int argc, char** argv)
{
	try {
		std::cout << term::EnableANSI;

		using namespace file::base;

		auto vec{ TkParse(Tkiz(file::read("test.txt")).tokenize()).parse() };

		return 0;
	} catch (const std::exception& ex) {
		std::cerr << term::error << ex.what() << std::endl;
		return -1;
	} catch (...) {
		std::cerr << term::error << "An unknown exception occurred!" << std::endl;
		return -2;
	}
}