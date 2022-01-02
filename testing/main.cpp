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

#include <xRand.hpp>

template<size_t CYCLE_COUNT>
inline std::array<int, CYCLE_COUNT> get_rng_num_instance(const int& min = 0, const int& max = 10000)
{
	std::array<int, CYCLE_COUNT> arr;
	memset(&arr, 0, CYCLE_COUNT);
	rng::xRand<std::mt19937, rng::seed::FromRandomDevice> rng;
	for (size_t i{ 0ull }; i < CYCLE_COUNT; ++i)
		arr[i] = rng.get(min, max);
	return arr;
}
template<size_t CYCLE_COUNT>
inline std::array<int, CYCLE_COUNT> get_rng_num_fresh(const int& min = 0, const int& max = 10000)
{
	std::array<int, CYCLE_COUNT> arr;
	memset(&arr, 0, CYCLE_COUNT);
	for (size_t i{ 0ull }; i < CYCLE_COUNT; ++i)
		arr[i] = rng::xRand<std::mt19937, rng::seed::FromRandomDevice>().get(min, max);
	return arr;
}


int main(const int argc, char** argv)
{
	try {
		#ifdef OS_WIN
		std::cout << term::EnableANSI;
		#endif

		using CLK = std::chrono::high_resolution_clock;
		using DurationT = std::chrono::duration<double, std::micro>;
		using TimePointT = std::chrono::time_point<CLK, DurationT>;

		const TimePointT t00{ CLK::now() };
		const auto r0{ get_rng_num_instance<2000ull>() };
		const TimePointT t01{ CLK::now() };

		const TimePointT t10{ CLK::now() };
		const auto r1{ get_rng_num_fresh<2000ull>() };
		const TimePointT t11{ CLK::now() };

		const DurationT t0{ t01 - t00 }, t1{ t11 - t10 };
		(std::cout << std::fixed).precision(4);
		std::cout << "Instance:  " << color::setcolor::cyan << t0 << color::setcolor::reset << '\n';
		std::cout << "Fresh:     " << color::setcolor::green << t1 << color::setcolor::reset << '\n';

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