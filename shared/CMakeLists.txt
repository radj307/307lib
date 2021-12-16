# shared
cmake_minimum_required(VERSION 3.15)
include(CMakeDependentOption)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
	# Fix MSVC __cplusplus macro
	string(APPEND CMAKE_CXX_FLAGS " /Zc:__cplusplus")
endif()

project("shared")

set(HEADERS
	"./include/sysarch.h"
	"./include/var.hpp"
	"./include/math.hpp"
	"./include/iter.hpp"
	"./include/make_exception.hpp"
)

cmake_dependent_option(SHARED_DISABLE_WINDOWS_HPP "Disable building the shared project with the Windows.hpp header." OFF WIN32 ON)
if (WIN32 AND NOT SHARED_DISABLE_WINDOWS_HPP)
	list(APPEND HEADERS "./include/Windows.hpp")
endif()

option(SHARED_ENABLE_XRAND "Enable the xRand.hpp randomization wrapper header." ON)
if (SHARED_ENABLE_XRAND)
	list(APPEND HEADERS "./include/xRand.hpp")
endif()

set(SRC
	"./src/var.cpp"
	"./src/make_exception.cpp"
)

add_library(shared STATIC ${HEADERS} ${SRC})

target_include_directories(shared PUBLIC "./include" "./src")
target_link_directories(shared PUBLIC "./src")

