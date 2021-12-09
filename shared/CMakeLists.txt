# shared
cmake_minimum_required(VERSION 3.13)
include(CMakeDependentOption)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
	# Fix MSVC __cplusplus macro
	string(APPEND CMAKE_CXX_FLAGS " /Zc:__cplusplus")
endif()

project("shared")

set(HEADERS
	"include/sysarch.h"
	"include/var.hpp"
	"include/make_exception.hpp"
)

cmake_dependent_option(SHARED_DISABLE_WINDOWS_HPP "Disable building the shared project with the Windows.hpp header." OFF WIN32 ON)
if (WIN32 AND NOT SHARED_DISABLE_WINDOWS_HPP)
	set(HEADERS ${HEADERS} "include/Windows.hpp")
endif()

option(ENABLE_XRAND "Enable the xRand.hpp randomization wrapper header." ON)
if (ENABLE_XRAND)
	set(HEADERS ${HEADERS} "include/xRand.hpp")
endif()

option(ENABLE_XLOG "Enable the xlog.hpp header." ON)
if (ENABLE_XLOG)
	set(HEADERS ${HEADERS} "include/xlog.hpp")
endif()

set(SOURCE
	"src/var.cpp"
	"src/make_exception.cpp"
)

add_library(shared STATIC ${HEADERS} ${SOURCE})

target_include_directories(shared PUBLIC "./include" "./src")

