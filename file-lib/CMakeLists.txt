# 307lib/file-lib
cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
	# Fix MSVC __cplusplus macro
	string(APPEND CMAKE_CXX_FLAGS " /Zc:__cplusplus")
endif()

project("file-lib")

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

add_library(file-lib STATIC ${HEADERS} ${SRC})
add_library(307lib::file-lib ALIAS file-lib)

target_include_directories(file-lib PUBLIC 
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
	"$<INSTALL_INTERFACE:include>"
)
target_link_directories(file-lib PUBLIC 
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"
	"$<INSTALL_INTERFACE:src>"
)

target_link_libraries(file-lib PUBLIC shared str-lib)

# Packaging
include(GenerateExportHeader)
generate_export_header(file-lib EXPORT_FILE_NAME "${CMAKE_CURRENT_SOURCE_DIR}/export.h")

if (307lib_ENABLE_PACKAGING)
	add_subdirectory("packaging")
endif()