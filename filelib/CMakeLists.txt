# 307lib/filelib
cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
	# Fix MSVC __cplusplus macro
	string(APPEND CMAKE_CXX_FLAGS " /Zc:__cplusplus")
endif()

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

	# New Tokenizer
	"include/TokenRedux.hpp"
)

file(GLOB SRC "src/*.cpp")
project("filelib" VERSION ${307lib_VERSION} LANGUAGES CXX)

add_library(filelib STATIC ${HEADERS} ${SRC})


target_include_directories(filelib PUBLIC 
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
	"$<INSTALL_INTERFACE:include>"
)

target_link_libraries(filelib PUBLIC shared strlib)

# Packaging
if (307lib_ENABLE_PACKAGING) # Include packaging
	GENERATE_PACKAGING(filelib)
	WRITE_VERSION_FILE(filelib SameMajorVersion)
	INSTALL_PACKAGE(filelib)
endif()