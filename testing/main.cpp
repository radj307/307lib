#include <TermAPI.hpp>
#include <xlog.hpp>

#include <fileio.hpp>
#include <TokenRedux.hpp>
#include <TokenReduxDefaultDefs.hpp>
#include <INIRedux.hpp>
#include <env.hpp>

#include <iostream>

#include <redirect.hpp>
#include <xlog.hpp>
#include <ArgumentRedux.hpp>

struct LogCap {
	std::stringstream stream;

	std::streambuf* rdbuf() { return stream.rdbuf(); }
	std::streambuf* rdbuf(auto&& buf) { return stream.rdbuf(std::forward<decltype(buf)>(buf)); }
};

int main(const int argc, char** argv)
{
	try {
		using term::setcolor;

		// CHECK ENV FOR TERM SUPPORT
		#ifdef OS_WIN
		//true
		#elif OS_LINUX
		if (const auto var{ env::getvar("TERM") }; var.has_value())
			std::cout << setcolor::green << var.value() << setcolor::reset_f << std::endl;
		else std::cout << setcolor::red << "[NOT FOUND]" << setcolor::reset_f << std::endl;
		#endif
		// CHECK ENV FOR TERM SUPPORT

		return 0;
	} catch (const std::exception& ex) {
		std::cerr << xlog::error << ex.what() << std::endl;
		return -1;
	} catch (...) {
		std::cerr << xlog::error << "An unknown exception occurred!" << std::endl;
		return -2;
	}
}