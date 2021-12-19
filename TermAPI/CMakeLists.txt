# 307lib/TermAPI
cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
	# Fix MSVC __cplusplus macro
	string(APPEND CMAKE_CXX_FLAGS " /Zc:__cplusplus")
endif()

project("TermAPI")

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
)
set(SRC
	# ANSI
	"src/Sequence.cpp"
	# color
	"src/setcolor.cpp"
	"src/palette.cpp"
	# term
	"src/term.cpp"
	# TermAPI
	"src/TermAPI.cpp"
)

include(CMakeDependentOption)

cmake_dependent_option(TERMAPI_EXTENSIONS_ENABLE_XLOG "Enable the xlog header." ON "ENABLE_TERMAPI;ENABLE_STRLIB" OFF)
if (TERMAPI_EXTENSIONS_ENABLE_XLOG)
	list(APPEND HEADERS "include/xlog.hpp")
	list(APPEND SRC "src/xlog.cpp")
endif()

cmake_dependent_option(TERMAPI_EXTENSIONS_ENABLE_COLORCONFIG "Enable the color-config header." ON "ENABLE_TERMAPI;ENABLE_STRLIB;ENABLE_FILELIB" OFF)
if (TERMAPI_EXTENSIONS_ENABLE_COLORCONFIG)
	list(APPEND HEADERS "include/color-config.hpp")
	list(APPEND SRC "src/color-config.cpp")
endif()

add_library(TermAPI STATIC ${HEADERS} ${SRC})
add_library(307lib::TermAPI ALIAS TermAPI)

if (TERMAPI_EXTENSIONS_ENABLE_XLOG)
	target_link_libraries(TermAPI PUBLIC strlib)
endif()
if (TERMAPI_EXTENSIONS_ENABLE_COLORCONFIG)
	target_link_libraries(TermAPI PUBLIC strlib filelib)
endif()

target_include_directories(TermAPI PUBLIC 
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include;${CMAKE_CURRENT_SOURCE_DIR}/ANSI/include>"
	"$<INSTALL_INTERFACE:ANSI/include;color/include;term/include;extensions/include>"
)
target_link_directories(TermAPI PUBLIC 
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src;${CMAKE_CURRENT_SOURCE_DIR}/ANSI/src>"
	"$<INSTALL_INTERFACE:ANSI/src;color/src;term/src;extensions/src;src>"
)

target_link_libraries(TermAPI PUBLIC ${LINKS})

# Packaging
include(GenerateExportHeader)
generate_export_header(TermAPI EXPORT_FILE_NAME "${CMAKE_CURRENT_SOURCE_DIR}/export.h")

# Include packaging by default if this is the top-level directory
if (307lib_ENABLE_PACKAGING)
	add_subdirectory("packaging")
endif()
