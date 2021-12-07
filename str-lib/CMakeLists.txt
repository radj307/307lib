cmake_minimum_required (VERSION 3.13)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
	# Fix MSVC __cplusplus macro
	string(APPEND CMAKE_CXX_FLAGS " /Zc:__cplusplus")
endif()

add_library(str-lib STATIC "strconv.hpp" "strmanip.hpp" "strutility.hpp" "strlocale.hpp" "strconcepts.hpp" "str.hpp")
set_target_properties(str-lib PROPERTIES LINKER_LANGUAGE CXX)
#target_include_directories(str-lib INTERFACE .)
