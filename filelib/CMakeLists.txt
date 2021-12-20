# 307lib/filelib
cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
	# Fix MSVC __cplusplus macro
	string(APPEND CMAKE_CXX_FLAGS " /Zc:__cplusplus")
endif()

set(filelib_VERSION_MAJOR 4)
set(filelib_VERSION_MINOR 0)
set(filelib_VERSION_PATCH 0)
set(filelib_VERSION ${filelib_VERSION_MAJOR}.${filelib_VERSION_MINOR}.${filelib_VERSION_PATCH})

project("filelib" VERSION ${filelib_VERSION} LANGUAGES CXX)

set(HEADERS
	# Base File Lib
	"include/fileio.hpp"
	"include/fileutil.hpp"
	"include/env.hpp"
	# Tokenizer Extensions
	"include/LEXEME.h"
	"include/Token.hpp"
	"include/TokenizerBase.hpp"
	"include/TokenParserBase.hpp"
	# INI File Format
	"include/INI_Tokenizer.hpp"
	"include/INI_Container.hpp"
	"include/INI_Parser.hpp"
	"include/INI.hpp"
)
set(SRC	
	"src/fileio.cpp"
	"src/fileutil.cpp"
	"src/env.cpp"
	"src/INI.cpp"
)

add_library(filelib STATIC ${HEADERS} ${SRC})
add_library(307lib::filelib ALIAS filelib)

target_include_directories(filelib PUBLIC 
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
	"$<INSTALL_INTERFACE:include>"
)
target_link_directories(filelib PUBLIC 
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"
	"$<INSTALL_INTERFACE:src>"
)

target_link_libraries(filelib PUBLIC shared strlib)

# Packaging
if (307lib_ENABLE_PACKAGING) # Include packaging
	GENERATE_PACKAGING(filelib)
	WRITE_VERSION_FILE(filelib SameMajorVersion)
	INSTALL_PACKAGE(filelib)
endif()