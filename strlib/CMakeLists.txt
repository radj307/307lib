﻿# 307lib/strlib
cmake_minimum_required (VERSION 3.13)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
	# Fix MSVC __cplusplus macro
	string(APPEND CMAKE_CXX_FLAGS " /Zc:__cplusplus")
endif()

set(strlib_VERSION_MAJOR 1)
set(strlib_VERSION_MINOR 0)
set(strlib_VERSION_PATCH 0)
set(strlib_VERSION ${strlib_VERSION_MAJOR}.${strlib_VERSION_MINOR}.${strlib_VERSION_PATCH})

project("strlib" VERSION ${strlib_VERSION})

set(HEADERS
	"./include/strconv.hpp"
	"./include/strmanip.hpp"
	"./include/strlocale.hpp"
	"./include/str.hpp"
	"./include/strutility.hpp"
)
set(SRC
	"./src/str.cpp"
)

add_library(strlib STATIC ${HEADERS} ${SRC})
target_link_libraries(strlib PUBLIC shared)
target_include_directories(strlib PUBLIC 
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
	"$<INSTALL_INTERFACE:include>"
)
target_link_directories(strlib PUBLIC 
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"
	"$<INSTALL_INTERFACE:src>"
)

# Packaging
include(GenerateExportHeader)
generate_export_header(strlib EXPORT_FILE_NAME "${CMAKE_CURRENT_SOURCE_DIR}/export.h")

# Include packaging by default if this is the top-level directory
if (307lib_ENABLE_PACKAGING)
	add_subdirectory("packaging")
endif()
