#include <strlocale.hpp>
#include <strutility.hpp>
#include <strconv.hpp>
#include <str.hpp>
#include <var.hpp>
#include <LineCharacter.hpp>
#include <fileio.hpp>
#include <fileutil.hpp>
#include <INI.hpp>
#include <process.hpp>

#include <TermAPI.hpp>
#include <xlog.hpp>
#include <color-transform.hpp>
#include <color-config.hpp>
#include <Argument.hpp>

#include "filelib.h"
#include "optlib.h"
#include "strlib.h"

int main(const int argc, char** argv)
{
	try {
		#ifdef OS_WIN
		std::cout << term::EnableANSI;
		#endif

		if (argc > 1) {
			opt::Argument arg{ argv[1] };

			opt::ParamsAPI2 args{ argc, argv, 'c', "capture" };
			for (auto& arg : args) {
				std::cout << "  " << arg << '\n';
			}
		}

		using namespace process;

		std::stringstream buffer;
		const std::string command{ "ls -lAg" };
		const auto rc{ exec(&buffer, command) };
		std::cout << '\"' << command << "\" returned " << rc << "\n\nCaptured STDOUT/STDERR:\n" << buffer.rdbuf() << '\n';

		return 0;
	} catch (const std::exception& ex) {
		std::cerr << term::error << ex.what() << std::endl;
		return -1;
	} catch (...) {
		std::cerr << term::error << "An unknown exception occurred!" << std::endl;
		return -2;
	}
}