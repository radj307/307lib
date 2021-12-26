# shared
cmake_minimum_required(VERSION 3.15)
include(CMakeDependentOption)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
	string(APPEND CMAKE_CXX_FLAGS " /Zc:__cplusplus") # Fix MSVC __cplusplus macro
endif()

set(shared_VERSION_MAJOR 1)
set(shared_VERSION_MINOR 0)
set(shared_VERSION_PATCH 0)
set(shared_VERSION ${shared_VERSION_MAJOR}.${shared_VERSION_MINOR}.${shared_VERSION_PATCH})

project("shared" VERSION ${shared_VERSION} LANGUAGES CXX)

set(HEADERS
	"include/sysarch.h"
	"include/var.hpp"
	"include/math.hpp"
	"include/iter.hpp"
	"include/make_exception.hpp"
	"include/procstream.hpp"
)

set(SRC
	"src/var.cpp"
	"src/make_exception.cpp"
)

option(SHARED_ENABLE_UNIT "Enable the unit.hpp header." OFF)
if (SHARED_ENABLE_UNIT)
	message(STATUS "unit.hpp is enabled.")
	list(APPEND HEADERS "include/unit.hpp")
	list(APPEND SRC "src/unit.cpp")
endif()

cmake_dependent_option(SHARED_ENABLE_WINDOWS_HPP "Disable building the shared project with the Windows.hpp header." ON "NOT WIN32" OFF)
if (SHARED_ENABLE_WINDOWS_HPP)
	message(STATUS "Windows.hpp is enabled.")
	list(APPEND HEADERS "include/Windows.hpp")
endif()

option(SHARED_ENABLE_XRAND "Enable the xRand.hpp randomization wrapper header." OFF)
if (SHARED_ENABLE_XRAND)
	message(STATUS "xRand.hpp is enabled.")
	list(APPEND HEADERS "include/xRand.hpp")
endif()

add_library(shared STATIC ${HEADERS} ${SRC})
add_library(307lib::shared ALIAS shared)

target_include_directories(shared PUBLIC 
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"	# absolute path used during build operation
	"$<INSTALL_INTERFACE:include>"								# relative path used during install operation
)
target_link_directories(shared PUBLIC 
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"		# absolute path used during build operation
	"$<INSTALL_INTERFACE:src>"									# relative path used during install operation
)

# Packaging
if (307lib_ENABLE_PACKAGING) # Include packaging
	GENERATE_PACKAGING(shared)
	WRITE_VERSION_FILE(shared SameMajorVersion)
	INSTALL_PACKAGE(shared)
endif()

