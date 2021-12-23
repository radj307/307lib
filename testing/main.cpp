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

int main(const int argc, char** argv)
{
	try {
		#ifdef OS_WIN
		std::cout << term::EnableANSI;
		#endif

		opt::ParamsAPI2 args{ argc, argv };
		for (auto pos{ args.find_any<opt::Flag>(std::make_tuple('a', 'b', 'c', 'd'), args.begin(), args.end())}, last{pos}; pos != args.end(); last = pos + 1, pos = args.find_any<opt::Flag>(std::make_tuple('a', 'b', 'c', 'd'), pos + 1ull, args.end())) {
			for (auto& it : args.get_range(last, pos)) {
				std::cout << it << '\n';
			}
		}
		

		std::cout << color::reset << std::endl;

		return 0;
	} catch (const std::exception& ex) {
		std::cerr << term::error << ex.what() << std::endl;
		return -1;
	} catch (...) {
		std::cerr << term::error << "An unknown exception occurred!" << std::endl;
		return -2;
	}
}