#include <TermAPI.hpp>

#include <fileio.hpp>
#include <TokenRedux.hpp>
#include <TokenReduxDefaultDefs.hpp>
#include <INIRedux.hpp>


int main(const int argc, char** argv)
{
	try {
		std::cout << term::EnableANSI;

		file::ini::INI ini{ "test.ini" };

		if constexpr (var::same_or_convertible<int, int>) {
			std::cout << ini << std::endl;
		}


		//file::write("test.txt", "Hello", ' ', "World!");
		//auto ss{ file::read("test.txt") };
		//std::cout << ss.rdbuf() << std::endl;
		//return 0;
		//using namespace file::base;

		//auto vec{ TkParse(Tkiz(file::read("test.txt")).tokenize()).parse() };

		return 0;
	} catch (const std::exception& ex) {
		std::cerr << term::error << ex.what() << std::endl;
		return -1;
	} catch (...) {
		std::cerr << term::error << "An unknown exception occurred!" << std::endl;
		return -2;
	}
}