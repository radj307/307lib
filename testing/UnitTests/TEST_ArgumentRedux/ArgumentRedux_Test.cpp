#include <gmock/gmock.h>
#include <ArgumentRedux.hpp>

#include <math.hpp>
#include <str.hpp>

#include <chrono>
#include <iostream>

using TIMEPRECISION_PERIOD = std::nano;
using TIMEPRECISION_T = std::chrono::nanoseconds;
using CLK = std::chrono::steady_clock;
using DUR = std::chrono::duration<double, TIMEPRECISION_PERIOD>;
using TIMEPCONT = std::vector<std::chrono::steady_clock::time_point>;
using TIMECONT = std::vector<DUR>;
#define TIMEP(test_number, object_name, identifier) t##test_number##_##object_name##_##identifier{ CLK::now() }
#define MAKE_DUR(test_number, object_name) t##test_number##_##object_name##_dur{ t##test_number##_##object_name##_##end - t##test_number##_##object_name##_##begin }
#define GET_DUR(test_number, object_name) t##test_number##_##object_name##_dur

#ifdef OS_WIN
#define SPACE_PRE "            "
#else
#define SPACE_PRE "                   "
#endif

using namespace opt;

class ArgumentRedux_Fixture : public ::testing::Test {
public:
	using ArgList = std::vector<std::string>;
	using CaptureList = ::opt::token::Parser::Input::invec;
private:
	static inline WINCONSTEXPR ArgList cat_args(ArgList&& left, const ArgList& right)
	{
		left.reserve(left.size() + right.size());
		for (auto& it : right)
			left.emplace_back(it);
		left.shrink_to_fit();
		return left;
	}
	template<size_t i = 0ull, typename... Ts>
	static inline WINCONSTEXPR ArgList&& cat_args(ArgList&& left, const std::tuple<Ts...>& vecs)
	{
		left = std::move(cat_args(std::forward<ArgList>(left), std::get<i>(vecs)));
		if constexpr (i + 1ull < sizeof...(Ts))
			return std::move(cat_args<i + 1ull>(std::forward<ArgList>(left), vecs));
		return std::move(left);
	}
	template<std::same_as<ArgList>... Ts>
	static inline WINCONSTEXPR ArgList&& cat_args(ArgList&& left, const Ts&... vecs) { return cat_args(std::forward<ArgList>(left), std::make_tuple(vecs...)); }

protected:
	ArgList _args_blanks{
		"    \t\t\t\t\t\t\t\t\t ",
		"    \t\v\r\nHello ",
		"\nµµµµ>#<qqqq\n",
		"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n \t\r\n\\",
		"                                                                                                                                                                                                            ",
		"                                                woah that's a long one ^																													shit this is too!"
	};
	ArgList _args_parameters{
		"Hello",
		"World",
		"Let's keep this continuous, yes?",
		"X:/_Workspace/_repo/_external/_even/_deeper/_into/_the/_abyss/_we/go.go",
		"Well",
		"-5417.6066135",
		"that's_SHIT.GOGGLES_in_ascii",
		"600613_73575",
		"now-try-and-figure-that-out"
	};
	ArgList _args_options{
		"--opt=Z:/_Workspace/_repo/_external/_even/_deeper/_into/_the/_abyss/_we/go.go",
		"--opt=Z:/directories with spaces in/their names/are a whole thing",
		"--a-really-long-option",
		"-5417.6066135",
		"--opt"
		"-5417.6066135",
		"--that's_SHIT.GOGGLES_in_ascii=",
		"HEY_THIS_ISNT_AN_OPTION!",
		"--y=n",
		"--opt=",
		"--opt=The previous option has a trailing space char.",
	};
	ArgList _args_flags{
		"-nNqQ=Why did the chicken cross the road?",
		"-Aa=to get to the other side",
		"-aA=to get hit on the other side",
		"-1FLAGS=mile!",
		"-S",
		"-Q",
		"To get to the kentacohut on the other side...",
		"...of life",
		"-P=ut",
		"-1.99",
		"-1.5",
		"___404error",
	};

	std::unique_ptr<ParamsAPI3> _inst_dummy;
	std::unique_ptr<ParamsAPI3> _inst_parameters;
	std::unique_ptr<ParamsAPI3> _inst_options;
	std::unique_ptr<ParamsAPI3> _inst_flags;
	std::unique_ptr<ParamsAPI3> _inst_all;

	void SetUp() override
	{
		_inst_dummy = std::make_unique<ParamsAPI3>(_args_blanks);
		_inst_parameters = std::make_unique<ParamsAPI3>(_args_parameters);
		_inst_options = std::make_unique<ParamsAPI3>(_args_options, "opt", 'y');
		_inst_flags = std::make_unique<ParamsAPI3>(_args_flags, 'S', 'A', 'Q');
		_inst_all = std::make_unique<ParamsAPI3>(cat_args(ArgList{}, _args_options, _args_parameters, _args_flags, _args_blanks), "opt", 'y', 'S', 'A', 'Q');
	}
	void TearDown() override
	{
		_inst_dummy.release();
		_inst_parameters.release();
		_inst_options.release();
		_inst_flags.release();
		_inst_all.release();
	}
public:
};

TEST_F(ArgumentRedux_Fixture, InitializationSpeed)
{
	const size_t cycleCount{ 100ull };
	const TIMEPRECISION_T average_less_than{ 30000 };

	std::cerr
		<< SPACE_PRE << "###################################################\n"
		<< SPACE_PRE << "#         Begin Initialization Speed Test         #\n"
		<< SPACE_PRE << "###################################################\n"
		<< SPACE_PRE << "TEST:     This test initializes 5 instances of ParamsAPI3 100 times each, and prints the average duration.\n"
		<< SPACE_PRE << "PURPOSE:  Gives a general idea of whether the initialization & parsing phases can complete without exception,\n"
		<< SPACE_PRE << "          and a general idea of the amount of time taken to initialize.\n"
		<< SPACE_PRE << "  Cycle Count:  " << cycleCount << '\n'
		<< SPACE_PRE << "  Max Average:  " << average_less_than.count() << " ns\n\n"
		;

	const auto& printSectionAverage{ [](std::string_view const& section_name, DUR const& duration) {
		std::cerr
			<< SPACE_PRE << section_name << " Average:"
			<< str::VIndent(24ull, section_name.size() + 10)
			<< duration.count() << " ns\n"
			;
	} };

	// Blanks
	TIMECONT t0_blanks;
	for (size_t i{ 0 }; i < cycleCount; ++i) {
		const auto TIMEP(i, blanks, begin);
		ASSERT_NO_THROW(ParamsAPI3(_args_blanks)) << "Initializing blanks caused an exception!";
		const auto TIMEP(i, blanks, end);
		const auto MAKE_DUR(i, blanks);
		t0_blanks.emplace_back(GET_DUR(i, blanks));
	}
	EXPECT_EQ(t0_blanks.size(), cycleCount) << "Incorrect number of test cycles!";
	const auto blanks_average{ math::average<DUR::rep, DUR::period>(t0_blanks) };
	EXPECT_LE(blanks_average, average_less_than) << "(blanks) average initialization time is longer than the maximum of " << average_less_than.count() << " ns";

	// Parameters
	TIMECONT t0_parameters;
	for (size_t i{ 0 }; i < cycleCount; ++i) {
		const auto TIMEP(i, parameters, begin);
		ASSERT_NO_THROW(ParamsAPI3(_args_parameters)) << "Initializing Parameters caused an exception!";
		const auto TIMEP(i, parameters, end);
		const auto MAKE_DUR(i, parameters);
		t0_parameters.emplace_back(GET_DUR(i, parameters));
	}
	EXPECT_EQ(t0_parameters.size(), cycleCount) << "Incorrect number of test cycles!";
	const auto parameters_average{ math::average<DUR::rep, DUR::period>(t0_parameters) };
	EXPECT_LE(parameters_average, average_less_than) << "(parameters) average initialization time is longer than the maximum of " << average_less_than.count() << " ns";

	// Options
	TIMECONT t0_options;
	for (size_t i{ 0 }; i < cycleCount; ++i) {
		const auto TIMEP(i, options, begin);
		ASSERT_NO_THROW(ParamsAPI3(_args_options, "opt", 'y')) << "Initializing Options caused an exception!";
		const auto TIMEP(i, options, end);
		const auto MAKE_DUR(i, options);
		t0_options.emplace_back(GET_DUR(i, options));
	}
	EXPECT_EQ(t0_options.size(), cycleCount) << "Incorrect number of test cycles!";
	const auto options_average{ math::average<DUR::rep, DUR::period>(t0_options) };
	EXPECT_LE(options_average, average_less_than) << "(options) average initialization time is longer than the maximum of " << average_less_than.count() << " ns";

	// Flags
	TIMECONT t0_flags;
	for (size_t i{ 0 }; i < cycleCount; ++i) {
		const auto TIMEP(i, flags, begin);
		ASSERT_NO_THROW(ParamsAPI3(_args_flags, 'S', 'A', 'Q')) << "Initializing Flags caused an exception!";
		const auto TIMEP(i, flags, end);
		const auto MAKE_DUR(i, flags);
		t0_flags.emplace_back(GET_DUR(i, flags));
	}
	EXPECT_EQ(t0_flags.size(), cycleCount) << "Incorrect number of test cycles!";
	const auto flags_average{ math::average<DUR::rep, DUR::period>(t0_flags) };
	EXPECT_LE(flags_average, average_less_than) << "(flags) average initialization time is longer than the maximum of " << average_less_than.count() << " ns";

	// All
	TIMECONT t0_all;
	for (size_t i{ 0 }; i < cycleCount; ++i) {
		const auto TIMEP(i, all, begin);
		ASSERT_NO_THROW(ParamsAPI3(_args_all)) << "Initializing All caused an exception!";
		const auto TIMEP(i, all, end);
		const auto MAKE_DUR(i, all);
		t0_all.emplace_back(GET_DUR(i, all));
	}
	EXPECT_EQ(t0_all.size(), cycleCount) << "Incorrect number of test cycles!";
	const auto all_average{ math::average<DUR::rep, DUR::period>(t0_all) };
	EXPECT_LE(all_average, average_less_than) << "(all) average initialization time is longer than the maximum of " << average_less_than.count() << " ns";


	printSectionAverage("Blanks", blanks_average);
	printSectionAverage("Parameters", parameters_average);
	printSectionAverage("Options (Cap)", options_average);
	printSectionAverage("Flags (Cap)", flags_average);
	printSectionAverage("All", all_average);
}

TEST_F(ArgumentRedux_Fixture, typegetv_all)
{
	std::cerr
		<< SPACE_PRE << "Begin typegetv_all Test.\n"
		;
	const auto blanks{ _inst_dummy.get()->typegetv_all<opt::Parameter>() };
	const auto parameters{ _inst_parameters.get()->typegetv_all<opt::Parameter>() };
	const auto options{ _inst_options.get()->typegetv_all<opt::Parameter>() };
	const auto flags{ _inst_flags.get()->typegetv_all<opt::Parameter>() };
	const auto all{ _inst_all.get()->typegetv_all<opt::Parameter>() };
	EXPECT_GT(all.size(), blanks.size());
	EXPECT_GT(all.size(), parameters.size());
	EXPECT_GT(all.size(), options.size());
	EXPECT_GT(all.size(), flags.size());
}

/// ArgumentRedux_Test MAIN
int main(int argc, char** argv)
{
	::testing::InitGoogleMock(&argc, argv);
	return RUN_ALL_TESTS();
}
