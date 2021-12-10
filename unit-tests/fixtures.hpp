#include <gtest/gtest.h>

#include <sysarch.h>

#include <Arg.hpp>
#include <ArgContainer.hpp>
#include <ArgParser.hpp>
#include <ParamsAPI2.hpp>

#include <str.hpp>

namespace opt {
	namespace unittest {
		class ArgContainerTest : public ::testing::Test {
		protected:
			Parameter p{ "my parameter" };
			Option o{ std::make_pair("option", "this must have been enclosed with quotes!") };
			Flag f{ std::make_pair('f', "captured-argument") };

			ArgContainer cont{
				p,
				o,
				f
			};
		public:
			~ArgContainerTest() noexcept = default;
		};
		class ArgParserTest : public ::testing::Test {
		protected:
			const std::string argument0{ "example" };
			const std::vector<std::string> argv{
				/*1*/	"my parameter",
				/*2*/	"--option",
				/*3*/	"\"this must have been enclosed with quotes!\"",
				/*4*/	"-f",
				/*5*/	"captured-argument",
				/*6*/	"--cap",
				/*7*/	"-0xFFD",
				/*8*/	"--cap",
				/*9*/	"--cap",
				/*10*/	"1.234",
				/*11*/	"--cap",
				/*12*/	"-1.234",
				/*>=13*/"-flags",
			};
			const opt::CaptureList cap{
				'f',
				"cap",
				"--option",
			};
		};
		class ParamsAPI2Test : public ::testing::Test {
		protected:
			ParamsAPI2 args{
				{ // args:
					"--opt",
					"capture-me",
					"-lf=asdfjkl;",
					"--hello",
					"world",
					"-1.23",
					"--opt",
					"-1.23",
					"--world",
					"hello?!!",
				},// captures:
				"opt",
				'f',
			};
		};
	}
}