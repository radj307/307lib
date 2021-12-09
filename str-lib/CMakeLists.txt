# str-lib
cmake_minimum_required (VERSION 3.13)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
	# Fix MSVC __cplusplus macro
	string(APPEND CMAKE_CXX_FLAGS " /Zc:__cplusplus")
endif()

project("str-lib")

set(HEADERS
	"./include/strutility.hpp"
	"./include/strconv.hpp"
	"./include/strmanip.hpp"
	"./include/strlocale.hpp"
	"./include/str.hpp"
)
set(SOURCE
	"./src/strutility.cpp"
)

add_library(str-lib STATIC ${HEADERS} ${SOURCE})
target_include_directories(str-lib PUBLIC ./include ./src)
set_target_properties(str-lib PROPERTIES LINKER_LANGUAGE CXX)
