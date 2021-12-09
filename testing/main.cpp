#include <strlocale.hpp>
#include <strutility.hpp>
#include <strconv.hpp>
#include <str.hpp>
#include <var.hpp>
#include <TermAPI.hpp>
#include <LineCharacter.hpp>

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
		xLog logger(LEVEL_ALL_NO_DEBUG);
		logger.log(LogLevel::INFO, "Hello", ' ', "World!");

		logger.setLevel(LEVEL_ALL);
		logger.log(LogLevel::WARNING, "The cheese is on fire!");
		logger.log(LogLevel::ERROR, "The cheese is combustable and may explode!");
		logger.log(LogLevel::CRITICAL, "The cheese has exploded!");

		return 0;
	} catch (const std::exception& ex) {
		std::cerr << sys::term::error << ex.what() << std::endl;
		return -1;
	} catch (...) {
		std::cerr << sys::term::error << "An unknown exception occurred!" << std::endl;
		return -2;
	}
}