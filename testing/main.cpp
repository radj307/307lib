#include <strlocale.hpp>
#include <strutility.hpp>
#include <strconv.hpp>
#include <str.hpp>
#include <var.hpp>
#include <TermAPI.hpp>
#include <LineCharacter.hpp>
#include <fileio.hpp>
#include <fileutil.hpp>
#include <INI.hpp>

#include "flagset.hpp"
#include "performance.hpp"
using namespace perf;

#define XLOG_INCLUDE_SELF
#include <xlog.hpp>

int main(const int argc, char** argv)
{
	try {
		std::cout << sys::term::EnableANSI;
		using namespace xlog;
		//std::ofstream ofs{ "testing.log" };
		//xLog<std::ofstream> logger{ ofs };
		xLog logger;
		logger.setLevel(level::ERROR | level::INFO | level::CRITICAL);
		logger.log(level::INFO, "Hello", ' ', "World!");

		logger << level::ERROR << "nothing " << "went wrong!" << msg_break;
		logger << level::CRITICAL << "Something ";
		logger << "may have happened..." << msg_break;

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