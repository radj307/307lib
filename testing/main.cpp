#include <strlocale.hpp>
#include <strutility.hpp>
#include <strconv.hpp>
#include <str.hpp>
#include <var.hpp>
#include <LineCharacter.hpp>
#include <fileio.hpp>
#include <fileutil.hpp>
#include <INI.hpp>

#include "flagset.hpp"
#include "performance.hpp"
using namespace perf;

#include <TermAPI.hpp>
#include <xlog.hpp>
#include <color-transform.hpp>

int main(const int argc, char** argv)
{
	try {
		std::cout << sys::term::EnableANSI;

		std::cout << color::setcolor(color::rgb_to_sgr(5, 0, 0)) << "red" << color::reset() << '\n';
		std::cout << color::setcolor(color::rgb_to_sgr(0, 5, 0)) << "green" << color::reset() << '\n';
		std::cout << color::setcolor(color::rgb_to_sgr(0, 0, 5)) << "blue" << color::reset() << '\n';

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

		return 0;
	} catch (const std::exception& ex) {
		std::cerr << sys::term::error << ex.what() << std::endl;
		return -1;
	} catch (...) {
		std::cerr << sys::term::error << "An unknown exception occurred!" << std::endl;
		return -2;
	}
}