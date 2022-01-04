# 307lib/TermAPI
cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
	# Fix MSVC __cplusplus macro
	string(APPEND CMAKE_CXX_FLAGS " /Zc:__cplusplus")
endif()

set(TermAPI_VERSION_MAJOR 4)
set(TermAPI_VERSION_MINOR 2)
set(TermAPI_VERSION_PATCH 0)
set(TermAPI_VERSION ${TermAPI_VERSION_MAJOR}.${TermAPI_VERSION_MINOR}.${TermAPI_VERSION_PATCH})

project("TermAPI" VERSION ${TermAPI_VERSION} LANGUAGES CXX)

set(HEADERS
	# ANSI
	"include/Segments.h"
	"include/Sequence.hpp"
	"include/winEnableANSI.hpp"
	"include/LineCharacter.hpp"
	# color
	"include/color-values.h"
	"include/color-transform.hpp"
	"include/color-format.hpp"
	"include/setcolor.hpp"
	"include/palette.hpp"
	# term
	"include/CursorOrigin.h"
	"include/term.hpp"
	# TermAPI
	"include/TermAPI.hpp"
	"include/WinTermAPI.hpp"
)

file(GLOB SRC "src/*.cpp")

include(CMakeDependentOption)

# extensions
set(ADDITIONAL_DEPENDENCIES)
cmake_dependent_option(TERMAPI_EXTENSIONS "Enable TermAPI extension headers that require additional dependencies." ON "ENABLE_TERMAPI" OFF)
if (TERMAPI_EXTENSIONS)
	# xlog
	cmake_dependent_option(TERMAPI_EXTENSIONS_XLOG "Enable the xLog TermAPI extension." ON "ENABLE_TERMAPI;TERMAPI_EXTENSIONS;ENABLE_STRLIB" OFF)
	if (TERMAPI_EXTENSIONS_XLOG)
		list(APPEND HEADERS "include/xlog.hpp")
		list(APPEND SRC "src/xlog.cpp")
		list(APPEND ADDITIONAL_DEPENDENCIES strlib)
	endif()
	# color-config
	cmake_dependent_option(TERMAPI_EXTENSIONS_COLORCONFIG "Enable the color-config TermAPI extension." ON "ENABLE_TERMAPI;TERMAPI_EXTENSIONS;ENABLE_STRLIB;ENABLE_FILELIB" OFF)
	if (TERMAPI_EXTENSIONS_COLORCONFIG)
		list(APPEND HEADERS "include/color-config.hpp")
		list(APPEND SRC "src/color-config.cpp")
		list(APPEND ADDITIONAL_DEPENDENCIES filelib) # filelib already requires strlib
	endif()
endif()

add_library(TermAPI STATIC ${HEADERS} ${SRC})

target_include_directories(TermAPI PUBLIC 
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include;${CMAKE_CURRENT_SOURCE_DIR}/src>"
	"$<INSTALL_INTERFACE:include;src>"
)

target_link_libraries(TermAPI PUBLIC shared ${ADDITIONAL_DEPENDENCIES})

# Packaging
if (307lib_ENABLE_PACKAGING) # Include packaging
	GENERATE_PACKAGING(TermAPI)
	WRITE_VERSION_FILE(TermAPI SameMajorVersion)
	INSTALL_PACKAGE(TermAPI)
endif()