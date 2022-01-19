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

#include <ParamsAPI2.hpp>

struct LogCap {
	std::stringstream stream;

	std::streambuf* rdbuf() { return stream.rdbuf(); }
	std::streambuf* rdbuf(auto&& buf) { return stream.rdbuf(std::forward<decltype(buf)>(buf)); }
};

using CLK = std::chrono::_V2::steady_clock;
using DUR = std::chrono::duration<double, std::nano>;
using TIMECONT = std::vector<std::chrono::_V2::steady_clock::time_point>;
#define TIMEP(test_number, object_name, identifier) t##test_number##_##object_name##_##identifier{ CLK::now() }
#define MAKE_DUR(test_number, object_name) t##test_number##_##object_name##_dur{ t##test_number##_##object_name##_##end - t##test_number##_##object_name##_##begin }
#define GET_DUR(test_number, object_name) t##test_number##_##object_name##_dur

int main(const int argc, char** argv)
{
	try {
		const auto TIMEP(0, v2, begin); // BEGIN
		opt::ParamsAPI2 args_v2{ argc, argv, "precision", "align-to" };
		const auto TIMEP(0, v2, end); // END
		const DUR MAKE_DUR(0, v2);

		const auto TIMEP(0, v3, begin); // BEGIN
		volatile opt::ParamsAPI3 args_v3{ argc, argv, "precision", "align-to" };
		const auto TIMEP(0, v3, end); // BEGIN
		const DUR MAKE_DUR(0, v3);


		const auto TIMEP(1, v2, begin); // BEGIN
		for (const auto& it : args_v2.typegetv_all<opt::Parameter>())
			(void)(it);
		const auto TIMEP(1, v2, end); // END
		const DUR MAKE_DUR(1, v2);

		const auto TIMEP(1, v3, begin); // BEGIN
		for (const auto& it : args_v3.typegetv_all<opt::Parameter>())
			(void)(it);
		const auto TIMEP(1, v3, end); // END
		const DUR MAKE_DUR(1, v3);

		std::cout
			<< "Initialization Speed:\n"
			<< "  " << "[v2]:    " << GET_DUR(0, v2).count() << " ns" << '\n'
			<< "  " << "[v3]:    " << GET_DUR(0, v3).count() << " ns" << '\n'
			<< "  " << "Diff:    " << math::difference<double, std::nano>(GET_DUR(0, v2), GET_DUR(0, v3)).count() << " ns" << '\n'
			<< "  " << "Winner:  " << (GET_DUR(0, v2).count() < GET_DUR(0, v3).count() ? "[v2]" : "[v3]")
			<< "\nTime to retrieve and iterate through all parameters:\n"
			<< "  " << "[v2]:    " << GET_DUR(1, v2).count() << " ns" << '\n'
			<< "  " << "[v3]:    " << GET_DUR(1, v3).count() << " ns" << '\n'
			<< "  " << "Diff:    " << math::difference<double, std::nano>(GET_DUR(1, v2), GET_DUR(1, v3)).count() << " ns" << '\n'
			<< "  " << "Winner:  " << (GET_DUR(1, v2).count() < GET_DUR(1, v3).count() ? "[v2]" : "[v3]")
			<< "\n\n"
			;

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