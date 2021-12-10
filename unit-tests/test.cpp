#include "fixtures.hpp"
using namespace opt;
using namespace opt::unittest;

#pragma region ArgContainerTest
TEST_F(ArgContainerTest, vector_functions) // Test the forwarded vector functions
{
	ASSERT_EQ(cont.size(), 3ull) << "Size reports incorrect size!";
	ASSERT_EQ(cont.at(1ull), *(cont.begin() + 1ll)) << "At or begin doesn't return the correct variable!";
	ASSERT_EQ(cont.rbegin().base(), cont.end()) << "Reverse begin does not return the vector ending!";
	ASSERT_EQ(cont.rend().base(), cont.begin()) << "Reverse end does not return the vector beginning!";
	ASSERT_EQ(cont.capacity(), 3ull) << "Vector capacity was not initialized correctly!";
	cont.reserve(cont.size() + 5ull);
	ASSERT_EQ(cont.capacity(), 8ull) << "Reserve didn't increase the vector's capacity by the expected amount!";
	ASSERT_FALSE(cont.empty()) << "Empty didn't return correctly!";
}

TEST_F(ArgContainerTest, arg0) // Test the arg0 functions
{
	ASSERT_EQ(cont.arg0(), std::nullopt) << "arg0 was not initialized correctly!";
	ASSERT_EQ(cont.arg0("Hello World!"), std::nullopt) << "arg0 setter did not return the expected value!";
	ASSERT_EQ(cont.arg0(), "Hello World!") << "arg0 setter did not set the value correctly!";
}

TEST_F(ArgContainerTest, find) // Test the find function
{
	const auto
		a{ cont.find("my parameter") },
		b{ cont.find<opt::Parameter>("my parameter") },
		c{ cont.find<opt::Option>("my parameter") };
	ASSERT_EQ(a, cont.begin());	// a == (first arg)
	ASSERT_EQ(a, b);			// a == b
	ASSERT_EQ(c, cont.end());	// c == (not found)
	ASSERT_EQ(cont.find('d'), c);// find('d') == c
}
TEST_F(ArgContainerTest, find_any) // Test the find_any function
{
	ASSERT_EQ(cont.find_any<opt::Parameter>("my parameter"), cont.begin());
	ASSERT_EQ(cont.find_any("my parameter"), cont.begin());
	ASSERT_EQ(cont.find_any('f'), cont.begin() + 2ll);
	ASSERT_EQ(cont.find_any<opt::Parameter>("option", 'f'), cont.end());
	ASSERT_EQ(cont.find_any<opt::Option>("option", 'f'), cont.begin() + 1ull);
	ASSERT_EQ(cont.find_any<opt::Flag>("option", 'f'), cont.begin() + 2ull);
}

TEST_F(ArgContainerTest, find_all) // Test the find_all function
{
	ASSERT_EQ(cont.find_all<opt::Option>(), opt::ArgContainerIteratorContainerType{ cont.begin() + 1ll });
	ASSERT_EQ(cont.find_all("my parameter", "option", 'f'), [this]() { opt::ArgContainerIteratorContainerType vec; vec.reserve(cont.size()); for (auto it{ cont.begin() }; it != cont.end(); ++it) vec.emplace_back(it); vec.shrink_to_fit(); return vec; }());
	const auto vec{ cont.find_all<opt::Option, opt::Parameter>() };
	const opt::ArgContainerIteratorContainerType comp{ cont.begin(), cont.begin() + 1ll };
	ASSERT_EQ(vec, comp);
}

TEST_F(ArgContainerTest, check) // Test the check function
{
	ASSERT_TRUE(cont.check("my parameter"));
	ASSERT_TRUE(cont.check("option"));
	ASSERT_TRUE(cont.check('f'));
	ASSERT_TRUE(cont.check<opt::Parameter>("my parameter"));
	ASSERT_TRUE(cont.check<opt::Option>("option"));
	ASSERT_TRUE(cont.check<opt::Flag>('f'));
	ASSERT_FALSE(cont.check("my parameter", cont.begin() + 1ll));
	ASSERT_FALSE(cont.check("option", cont.begin() + 2ll));
	ASSERT_FALSE(cont.check('f', cont.begin(), cont.begin() + 2ll));
	ASSERT_FALSE(cont.check<opt::Flag>("my parameter"));
	ASSERT_FALSE(cont.check<opt::Parameter>("option"));
	ASSERT_FALSE(cont.check<opt::Option>('f'));
	ASSERT_FALSE((cont.check<opt::Flag, opt::Option>("my parameter")));
	ASSERT_FALSE((cont.check<opt::Parameter, opt::Flag>("option")));
	ASSERT_FALSE((cont.check<opt::Option, opt::Parameter>('f')));
	ASSERT_TRUE(cont.check());

	ASSERT_FALSE(cont.check("facsimile"));
	ASSERT_FALSE(cont.check("hello world!"));
	ASSERT_FALSE(cont.check('a'));
	ASSERT_FALSE(cont.check(""));
	ASSERT_FALSE(cont.check('\033'));
}

TEST_F(ArgContainerTest, check_any) // Test the check_any function
{
	ASSERT_TRUE(cont.check_any());
	ASSERT_TRUE(cont.check_any<opt::Parameter>());
	ASSERT_TRUE(cont.check_any<opt::Option>());
	ASSERT_TRUE(cont.check_any<opt::Flag>());
	ASSERT_TRUE(cont.check_any<opt::Parameter>("my parameter"));
	ASSERT_TRUE(cont.check_any<opt::Option>("option"));
	ASSERT_TRUE(cont.check_any<opt::Flag>('f'));

	ASSERT_FALSE(cont.check_any("facsimile", 'd', "hello"));
	ASSERT_FALSE(cont.check_any<opt::Parameter>("option", 'd', "hello"));
	ASSERT_FALSE(cont.check_any<opt::Option>("my parameter", 'd', "hello"));
}

TEST_F(ArgContainerTest, check_all) // Test the check_all function
{
	ASSERT_TRUE(cont.check_all("f"));
	ASSERT_TRUE(cont.check_all("my parameter", "option", 'f'));
	ASSERT_TRUE(cont.check_all("my parameter", 'f'));
	ASSERT_TRUE(cont.check_all("my parameter", "option"));
	ASSERT_TRUE(cont.check_all("my parameter"));
	ASSERT_TRUE(cont.check_all("my parameter", "my parameter", "my parameter", "my parameter", "my parameter"));
	ASSERT_FALSE(cont.check_all("my parameter", "", 'f'));
	ASSERT_FALSE(cont.check_all("my parameter", ' '));
	ASSERT_FALSE(cont.check_all("my parameter", "optio"));
}

TEST_F(ArgContainerTest, get)
{
	ASSERT_NO_THROW(ASSERT_EQ(opt::get<opt::Parameter>(cont.get("my parameter").value()), p));
	ASSERT_NO_THROW(ASSERT_EQ(opt::get<opt::Option>(cont.get("option").value()), o));
	ASSERT_NO_THROW(ASSERT_EQ(opt::get<opt::Flag>(cont.get('f').value()), f));
	ASSERT_EQ(cont.get("my parameter", cont.begin() + 1ll), std::nullopt);
	ASSERT_EQ(cont.get("facsimile"), std::nullopt);
	ASSERT_EQ(cont.get('d'), std::nullopt);
}
TEST_F(ArgContainerTest, get_any)
{
	ASSERT_NO_THROW(ASSERT_EQ(cont.get_any<opt::Parameter>().value(), *cont.begin()));
	ASSERT_NO_THROW(ASSERT_EQ(cont.get_any().value(), *cont.begin()));
	ASSERT_NO_THROW(ASSERT_FALSE(cont.get_any<opt::Flag>("my parameter").has_value()));
	ASSERT_NO_THROW(ASSERT_EQ(cont.get_any<opt::Option>(), *(cont.begin() + 1ll)));
	ASSERT_NO_THROW(ASSERT_EQ(cont.get_any("option", 'f'), *(cont.begin() + 1ll)));
}
TEST_F(ArgContainerTest, get_all)
{
	ASSERT_EQ(cont.get_all(), cont.operator const opt::ArgContainerType());
	ASSERT_EQ(cont.get_all<opt::Parameter>(), std::vector<opt::VariantArgumentType>{p});
	ASSERT_EQ(cont.get_all<opt::Option>(), std::vector<opt::VariantArgumentType>{o});
	ASSERT_EQ(cont.get_all<opt::Flag>(), std::vector<opt::VariantArgumentType>{f});

	const auto vec{ cont.get_all<opt::Parameter, opt::Flag>() };
	const opt::ArgContainerType comp{ p, f };
	ASSERT_EQ(vec, comp);
}

TEST_F(ArgContainerTest, get_range)
{
	ASSERT_EQ(cont.get_range(cont.begin()), cont.operator const opt::ArgContainerType());
	opt::ArgContainerType comprange{ p, o };
	ASSERT_EQ(cont.get_range(cont.begin(), cont.find('f'), false), comprange) << "Exclusive get_range failed!";
	ASSERT_EQ(cont.get_range(cont.begin(), cont.find("option")), comprange) << "Inclusive get_range failed!";
	ASSERT_EQ(cont.get_range(cont.find("my parameter"), cont.find('f'), false), comprange) << "Exclusive get_range failed!";
	ASSERT_EQ(cont.get_range(cont.find("my parameter"), cont.find("option")), comprange) << "Inclusive get_range failed!";
	comprange = { o, f };
	ASSERT_EQ(cont.get_range(cont.begin() + 1ll), comprange) << "Incorrect value deduced from std::nullopt end iterator.";
	ASSERT_ANY_THROW(cont.get_range(cont.end(), cont.begin()));
	comprange = { o };
	ASSERT_NO_THROW(ASSERT_EQ(cont.get_range(cont.begin() + 1ll, cont.begin() + 1ll), comprange));
}
#pragma endregion ArgContainerTest

#pragma region ArgParserTest
TEST_F(ArgParserTest, is_number)
{
	ASSERT_NO_THROW(ASSERT_TRUE(opt::is_number("-0xADFF04")));
	ASSERT_NO_THROW(ASSERT_FALSE(opt::is_number("-ADFF04")));
	ASSERT_NO_THROW(ASSERT_TRUE(opt::is_number("0xADFF04")));
	ASSERT_NO_THROW(ASSERT_FALSE(opt::is_number("ADFF04")));
	ASSERT_NO_THROW(ASSERT_TRUE(opt::is_number("-0xadff04")));
	ASSERT_NO_THROW(ASSERT_FALSE(opt::is_number("-adff04")));
	ASSERT_NO_THROW(ASSERT_TRUE(opt::is_number("0xadff04")));
	ASSERT_NO_THROW(ASSERT_FALSE(opt::is_number("adff04")));
	ASSERT_NO_THROW(ASSERT_TRUE(opt::is_number("-0xaDfF04")));
	ASSERT_NO_THROW(ASSERT_FALSE(opt::is_number("-aDfF04")));
	ASSERT_NO_THROW(ASSERT_TRUE(opt::is_number("0xAdFf04")));
	ASSERT_NO_THROW(ASSERT_FALSE(opt::is_number("AdFf04")));
	ASSERT_NO_THROW(ASSERT_TRUE(opt::is_number("1024313543451353415631123")));
	ASSERT_NO_THROW(ASSERT_FALSE(opt::is_number("1231f454fad231")));
	ASSERT_NO_THROW(ASSERT_TRUE(opt::is_number("-1024313543451353415631123")));
	ASSERT_NO_THROW(ASSERT_FALSE(opt::is_number("-1024313543451353F415631123")));
	ASSERT_NO_THROW(ASSERT_TRUE(opt::is_number("1024.313543451353415631123")));
	ASSERT_NO_THROW(ASSERT_FALSE(opt::is_number("1231f454f.ad231")));
	ASSERT_NO_THROW(ASSERT_TRUE(opt::is_number("-1.024313543451353415631123")));
	ASSERT_NO_THROW(ASSERT_FALSE(opt::is_number("-1.024313543451353F415631123")));
	ASSERT_NO_THROW(ASSERT_FALSE(opt::is_number("")));
	ASSERT_NO_THROW(ASSERT_FALSE(opt::is_number("\033")));
	ASSERT_NO_THROW(ASSERT_FALSE(opt::is_number("                   ")));
	ASSERT_NO_THROW(ASSERT_FALSE(opt::is_number("\t")));
	ASSERT_NO_THROW(ASSERT_FALSE(opt::is_number("\n")));
	ASSERT_NO_THROW(ASSERT_FALSE(opt::is_number("  \t")));
	ASSERT_NO_THROW(ASSERT_FALSE(opt::is_number("  \n")));
}

TEST_F(ArgParserTest, can_capture_next)
{
	auto it{ std::find(argv.begin(), argv.end(), "--option") };
	ASSERT_NO_THROW(ASSERT_TRUE(opt::can_capture_next(it, argv.end())));
	it = std::find(argv.begin(), argv.end(), "--cap");
	ASSERT_NE(it, argv.end()) << "Argument vector was modified and \"--cap\" wasn't found!";
	ASSERT_NO_THROW(ASSERT_TRUE(opt::can_capture_next(it, argv.end())));
	it = std::find(it + 1ll, argv.end(), "--cap");
	ASSERT_NE(it, argv.end()) << "Argument vector was modified and \"--cap\" wasn't found!";
	ASSERT_NO_THROW(ASSERT_FALSE(opt::can_capture_next(it, argv.end())) << "Argument \"--cap\" captured itself!");
	it = std::find(it + 1ll, argv.end(), "--cap");
	ASSERT_NE(it, argv.end()) << "Argument vector was modified and \"--cap\" wasn't found!";
	ASSERT_NO_THROW(ASSERT_TRUE(opt::can_capture_next(it, argv.end())));
	it = std::find(it + 1ll, argv.end(), "--cap");
	ASSERT_NE(it, argv.end()) << "Argument vector was modified and \"--cap\" wasn't found!";
	ASSERT_NO_THROW(ASSERT_TRUE(opt::can_capture_next(it, argv.end())));
}

TEST_F(ArgParserTest, count_prefix)
{
	ASSERT_NO_THROW(ASSERT_EQ(opt::count_prefix("--cap", 2ull), 2ull));
	ASSERT_NO_THROW(ASSERT_EQ(opt::count_prefix("---cap", 2ull), 2ull));
	ASSERT_NO_THROW(ASSERT_EQ(opt::count_prefix("---cap", 15ull), 3ull));
	ASSERT_NO_THROW(ASSERT_EQ(opt::count_prefix("-/-----///-cap", 2ull), 1ull));
	ASSERT_NO_THROW(ASSERT_EQ(opt::count_prefix("-cap", 2ull), 1ull));
	Settings_ArgParser.delimiters = { '/' };
	ASSERT_NO_THROW(ASSERT_EQ(opt::count_prefix("-cap", 2ull), 0ull));
	Settings_ArgParser.delimiters = { '-' };
	ASSERT_NO_THROW(ASSERT_EQ(opt::count_prefix("cap", 2ull), 0ull));
}
TEST_F(ArgParserTest, strip_prefix)
{
	ASSERT_NO_THROW(ASSERT_EQ(opt::strip_prefix("--cap", 2ull), std::make_pair("cap"s, 2ull)));
	ASSERT_NO_THROW(ASSERT_EQ(opt::strip_prefix("---cap", 2ull), std::make_pair("-cap"s, 2ull)));
	ASSERT_NO_THROW(ASSERT_EQ(opt::strip_prefix("---cap", 15ull), std::make_pair("cap"s, 3ull)));
	Settings_ArgParser.delimiters = { '-', '/' };
	ASSERT_NO_THROW(ASSERT_EQ(opt::strip_prefix("-/-----///-cap", 2ull), std::make_pair("-----///-cap"s, 2ull)));
	Settings_ArgParser.delimiters = { '-' };
	ASSERT_NO_THROW(ASSERT_EQ(opt::strip_prefix("-cap", 2ull), std::make_pair("cap"s, 1ull)));
	Settings_ArgParser.delimiters = { '/' };
	ASSERT_NO_THROW(ASSERT_EQ(opt::strip_prefix("-cap", 2ull), std::make_pair("-cap"s, 0ull)));
	Settings_ArgParser.delimiters = { '-' };
	ASSERT_NO_THROW(ASSERT_EQ(opt::strip_prefix("cap", 2ull), std::make_pair("cap"s, 0ull)));
}

TEST_F(ArgParserTest, CaptureList)
{
	ASSERT_NO_THROW(ASSERT_TRUE(cap.is_present("cap")));
	ASSERT_NO_THROW(ASSERT_TRUE(cap.is_present("option")));
	ASSERT_NO_THROW(ASSERT_TRUE(cap.is_present("--cap", '-')));
	ASSERT_NO_THROW(ASSERT_TRUE(cap.is_present("--option", '-')));
}

TEST_F(ArgParserTest, parse_GeneralLogic)
{
	const ArgContainer cont{ opt::parse(argv, cap), argument0 };
	size_t i{ 0ull };
	ASSERT_NO_THROW(ASSERT_EQ(cont.at(i++), (opt::VariantArgumentType{ opt::Parameter{ "my parameter" } })));
	ASSERT_NO_THROW(ASSERT_EQ(cont.at(i++), (opt::VariantArgumentType{ opt::Option{ "option", "\"this must have been enclosed with quotes!\"" } })));
	ASSERT_NO_THROW(ASSERT_EQ(cont.at(i++), (opt::VariantArgumentType{ opt::Flag{ 'f', "captured-argument" } })));
	ASSERT_NO_THROW(ASSERT_EQ(cont.at(i++), (opt::VariantArgumentType{ opt::Option{ "cap", "-0xFFD" } })));
	ASSERT_NO_THROW(ASSERT_EQ(cont.at(i++), (opt::VariantArgumentType{ opt::Option{ "cap", std::nullopt } })));
	ASSERT_NO_THROW(ASSERT_EQ(cont.at(i++), (opt::VariantArgumentType{ opt::Option{ "cap", "1.234" } })));
	ASSERT_NO_THROW(ASSERT_EQ(cont.at(i++), (opt::VariantArgumentType{ opt::Option{ "cap", "-1.234" } })));
	ASSERT_NO_THROW(ASSERT_EQ(cont.at(i++), (opt::VariantArgumentType{ opt::Flag{ 'f', std::nullopt } })));
	ASSERT_NO_THROW(ASSERT_EQ(cont.at(i++), (opt::VariantArgumentType{ opt::Flag{ 'l', std::nullopt } })));
	ASSERT_NO_THROW(ASSERT_EQ(cont.at(i++), (opt::VariantArgumentType{ opt::Flag{ 'a', std::nullopt } })));
	ASSERT_NO_THROW(ASSERT_EQ(cont.at(i++), (opt::VariantArgumentType{ opt::Flag{ 'g', std::nullopt } })));
	ASSERT_NO_THROW(ASSERT_EQ(cont.at(i++), (opt::VariantArgumentType{ opt::Flag{ 's', std::nullopt } })));
	ASSERT_EQ(i, cont.size()) << "Did the size of ArgParserTest.argv change?";
}

TEST_F(ArgParserTest, parse_FlagCaptureLogic)
{
	const ArgContainer cont{ opt::parse({ // Argv:
		"-ru",
		"capturable",
		"-ur",
		"not-capturable",
		"-1.2",
		"-f1",
		"-444.444"
		}, { // Cap:
			'u',
			'1',
		}), argument0 };
	size_t i{ 0ull };
	ASSERT_NO_THROW(ASSERT_EQ(cont.at(i++), (opt::VariantArgumentType{ opt::Flag{ 'r', std::nullopt } })) << "First argument \'r\' is invalid. Check flag parsing logic!") << "Exception thrown!";
	ASSERT_NO_THROW(ASSERT_EQ(cont.at(i++), (opt::VariantArgumentType{ opt::Flag{ 'u', "capturable" } })) << "Second argument \'u\' is invalid. Check flag parsing logic!") << "Exception thrown!";
	ASSERT_NO_THROW(ASSERT_EQ(cont.at(i++), (opt::VariantArgumentType{ opt::Flag{ 'u', std::nullopt } })) << "Third argument \'u\' is invalid. Check flag parsing logic!") << "Exception thrown!";
	ASSERT_NO_THROW(ASSERT_EQ(cont.at(i++), (opt::VariantArgumentType{ opt::Flag{ 'r', std::nullopt } })) << "Fourth argument \'r\' is invalid. Check flag parsing logic!") << "Exception thrown!";
	ASSERT_NO_THROW(ASSERT_EQ(cont.at(i++), (opt::VariantArgumentType{ opt::Parameter{ "not-capturable" } })) << "Fifth argument \"not-capturable\" is invalid. Check flag capturing & parameter parsing logic!") << "Exception thrown!";
	ASSERT_NO_THROW(ASSERT_EQ(cont.at(i++), (opt::VariantArgumentType{ opt::Parameter{ "-1.2" } })) << "") << "Exception thrown!";
	ASSERT_NO_THROW(ASSERT_EQ(cont.at(i++), (opt::VariantArgumentType{ opt::Flag{ 'f', std::nullopt } })) << "") << "Exception thrown!";
	ASSERT_NO_THROW(ASSERT_EQ(cont.at(i++), (opt::VariantArgumentType{ opt::Flag{ '1', "-444.444" } })) << "") << "Exception thrown!";
	ASSERT_EQ(i, cont.size()) << "Did the size of the argument vector change? Index is set to an unexpected value after all other tests passed!";
}

TEST_F(ArgParserTest, parse_EqualsCaptureLogic)
{
	const ArgContainer cont{ opt::parse({ // argv
		/*0*/		"--opt=-123",
		/*1*/		"--opt=\"Hello World!\"",
		/*2*/		"--opt=...",
		/*3-7*/		"-flags=303",
		/*8*/		"-s=adist",
		/*9-11*/	"-sg=lingen",
		}, {// cap
			"opt",
			's',
		}), argument0 };
	size_t i{ 0ull };
	ASSERT_NO_THROW(ASSERT_EQ(cont.at(i++), (opt::VariantArgumentType{ opt::Option{ "opt", "-123" } })));
	ASSERT_NO_THROW(ASSERT_EQ(cont.at(i++), (opt::VariantArgumentType{ opt::Option{ "opt", "\"Hello World!\"" } })));
	ASSERT_NO_THROW(ASSERT_EQ(cont.at(i++), (opt::VariantArgumentType{ opt::Option{ "opt", "..." } })));
	ASSERT_NO_THROW(ASSERT_EQ(cont.at(i++), (opt::VariantArgumentType{ opt::Flag{ 'f', std::nullopt } })));
	ASSERT_NO_THROW(ASSERT_EQ(cont.at(i++), (opt::VariantArgumentType{ opt::Flag{ 'l', std::nullopt } })));
	ASSERT_NO_THROW(ASSERT_EQ(cont.at(i++), (opt::VariantArgumentType{ opt::Flag{ 'a', std::nullopt } })));
	ASSERT_NO_THROW(ASSERT_EQ(cont.at(i++), (opt::VariantArgumentType{ opt::Flag{ 'g', std::nullopt } })));
	ASSERT_NO_THROW(ASSERT_EQ(cont.at(i++), (opt::VariantArgumentType{ opt::Flag{ 's', "303" } })));
	ASSERT_NO_THROW(ASSERT_EQ(cont.at(i++), (opt::VariantArgumentType{ opt::Flag{ 's', "adist" } })));
	ASSERT_NO_THROW(ASSERT_EQ(cont.at(i++), (opt::VariantArgumentType{ opt::Flag{ 's', std::nullopt } })));
	ASSERT_NO_THROW(ASSERT_EQ(cont.at(i++), (opt::VariantArgumentType{ opt::Flag{ 'g', std::nullopt } })));
	ASSERT_NO_THROW(ASSERT_EQ(cont.at(i++), (opt::VariantArgumentType{ opt::Parameter{ "lingen" } })));
	ASSERT_EQ(i, cont.size()) << "Did the size of the argument vector change? Index is set to an unexpected value after all other tests passed!";
}
#pragma endregion ArgParserTest

#pragma region ParamsAPI2Test
TEST_F(ParamsAPI2Test, typeget)
{
	ASSERT_NO_THROW(ASSERT_EQ((args.typeget<opt::Parameter>("world").value()), opt::Parameter{ "world" }));
	ASSERT_NO_THROW(ASSERT_EQ((args.typeget<opt::Parameter>("-1.23").value()), opt::Parameter{ "-1.23" }));
	ASSERT_NO_THROW(ASSERT_EQ((args.typeget<opt::Parameter>("hello?!!").value()), opt::Parameter{ "hello?!!" }));
}
#pragma endregion ParamsAPI2Test