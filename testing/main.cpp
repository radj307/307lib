#include <strlocale.hpp>
#include <strutility.hpp>
#include <strconv.hpp>
#include <str.hpp>
#include <var.hpp>
#include <LineCharacter.hpp>
#include <fileio.hpp>
#include <fileutil.hpp>
#include <INI.hpp>

#include <TermAPI.hpp>
#include <xlog.hpp>
#include <color-transform.hpp>
#include <color-config.hpp>

#include "filelib.h"
#include "optlib.h"
#include "strlib.h"
#include "TermAPI.h"

inline void test_xlog()
{
	std::cout << color::setcolor(color::rgb_to_sgr(5, 0, 0)) << "red" << color::reset << '\n';
	std::cout << color::setcolor(color::rgb_to_sgr(0, 5, 0)) << "green" << color::reset << '\n';
	std::cout << color::setcolor(color::rgb_to_sgr(0, 0, 5)) << "blue" << color::reset << '\n';
	std::cout << color::setcolor(color::rgb_to_sgr(3, 0, 5)) << "magenta" << color::reset << '\n';
	std::cout << color::setcolor(3, 2, 5) << "purple" << color::reset << '\n';

	if (str::pos_valid(std::string::npos)) {
		std::cout << "Impossible!" << std::endl;
	}
	using namespace xlog;
	xLogs logger;
	logger.setLevel(level::ERROR | level::INFO | level::CRITICAL);
	logger.log(level::INFO, "Hello", ' ', "World!");

	logger << level::ERROR << "nothing " << "went wrong!" << endm;
	logger << level::CRITICAL << "Something ";
	logger << "may have happened..." << endm;

	logger.setLevel(level::AllDebug);
	logger.log(level::DEBUG, "Initialize cheese scenario");

	logger.log(level::LOG, "The cheese is here!");
	logger.log(level::WARNING, "The cheese is on fire!");
	logger.log(level::ERROR, "The cheese is combustable and may explode!");
	logger.log(level::CRITICAL, "The cheese has exploded!");

	logger.log(level::DEBUG, "Cheese scenario ended, catastrophic failure!");
}

int main(const int argc, char** argv)
{
	try {
	#ifdef OS_WIN
		std::cout << term::EnableANSI;
	#endif
		color::Config cfg{ "test.txt" };
		std::cout << cfg.set("c1", "c2", "c4") << "Hello" << cfg.reset() << '\n';


		return 0;
	} catch (const std::exception& ex) {
		std::cerr << term::error << ex.what() << std::endl;
		return -1;
	} catch (...) {
		std::cerr << term::error << "An unknown exception occurred!" << std::endl;
		return -2;
	}
}