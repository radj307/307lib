#include <TermAPI.hpp>

#include <TokenRedux.hpp>

int main(const int argc, char** argv)
{
	try {
		std::cout << term::EnableANSI;



		return 0;
	} catch (const std::exception& ex) {
		std::cerr << term::error << ex.what() << std::endl;
		return -1;
	} catch (...) {
		std::cerr << term::error << "An unknown exception occurred!" << std::endl;
		return -2;
	}
}