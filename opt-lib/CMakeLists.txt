# opt-lib
cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
	# Fix MSVC __cplusplus macro
	string(APPEND CMAKE_CXX_FLAGS " /Zc:__cplusplus")
endif()

project("opt-lib")

set(HEADERS
	"./include/Arg.hpp"
	"./include/ArgContainer.hpp"
	"./include/ArgParser.hpp"
	"./include/VariantArgumentType.hpp"
	"./include/env.hpp"
	"./include/ParamsAPI2.hpp"
)
set(SOURCE
	"./src/env.cpp"
	"./src/ParamsAPI2.cpp"
)

add_library(opt-lib STATIC ${HEADERS} ${SOURCE})
# link dependencies
target_link_libraries(opt-lib PUBLIC shared str-lib file-lib)
# include directories
target_include_directories(opt-lib PUBLIC "./include" "./src")

