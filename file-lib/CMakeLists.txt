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
	"include/fileio.hpp"
	"include/fileutil.hpp"
	"include/env.hpp"
	"include/token/LEXEME.h"
	"include/token/Token.hpp"
	"include/token/TokenizedContainer.hpp"
	"include/token/TokenizerBase.hpp"
	"include/token/TokenizerINI.hpp"
	"include/token/TokenizerJSON.hpp"
	"include/token/TokenizerRE.hpp"

	"include/container/ContainerINI.hpp"
	"include/container/INIContainer.hpp"
	"include/container/INIContainerMonoType.hpp"
	"include/container/REContainer.hpp"

	"include/parser/INIParser.hpp"
	"include/parser/INIParserMonoType.hpp"
	"include/parser/REParser.hpp"

	"include/INI.hpp"
)
set(SRC	
	"src/fileutil.cpp"
	"src/env.cpp"
)
if(FILELIB_ENABLE_EXTENSIONS)
	list(APPEND HEADERS 
	)
endif()

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