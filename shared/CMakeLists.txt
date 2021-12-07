cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
	# Fix MSVC __cplusplus macro
	string(APPEND CMAKE_CXX_FLAGS " /Zc:__cplusplus")
endif()

add_library(shared STATIC "sysarch.h" "var.hpp" "make_exception.hpp")
target_include_directories(shared PUBLIC .)
set_target_properties(shared PROPERTIES LINKER_LANGUAGE CXX)

