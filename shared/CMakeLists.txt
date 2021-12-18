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
include(GenerateExportHeader)
generate_export_header(shared EXPORT_FILE_NAME "${CMAKE_CURRENT_SOURCE_DIR}/export.h")

# Include packaging by default if this is the top-level directory
if (307lib_ENABLE_PACKAGING)
	add_subdirectory("packaging")
endif()

