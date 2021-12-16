# file-lib
cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
	# Fix MSVC __cplusplus macro
	string(APPEND CMAKE_CXX_FLAGS " /Zc:__cplusplus")
endif()

project("file-lib")

option(FILELIB_ENABLE_EXTENSIONS "Enable extensions of the file-lib library, including tokenization/parsing capabilities & more. If this is disabled, only fileio & fileutil will be available." ON)

set(HEADERS
	"./include/fileio.hpp"
	"./include/fileutil.hpp"
	"./include/env.hpp"
)
set(SRC
	
	"./src/fileutil.cpp"
	"./src/env.cpp"
)
if(FILELIB_ENABLE_EXTENSIONS)
	list(APPEND HEADERS 
		"./include/token/LEXEME.h"
		"./include/token/Token.hpp"
		"./include/token/TokenizedContainer.hpp"
		"./include/token/TokenizerBase.hpp"
		"./include/token/TokenizerINI.hpp"
		"./include/token/TokenizerJSON.hpp"
		"./include/token/TokenizerRE.hpp"

		"./include/container/ContainerINI.hpp"
		"./include/container/INIContainer.hpp"
		"./include/container/INIContainerMonoType.hpp"
		"./include/container/REContainer.hpp"

		"./include/parser/INIParser.hpp"
		"./include/parser/INIParserMonoType.hpp"
		"./include/parser/REParser.hpp"
	)
endif()

add_library(file-lib STATIC ${HEADERS} ${SRC})

target_include_directories(file-lib PUBLIC "./include" "./src") # include "./src" here so source files can add headers directly
target_link_directories(file-lib PUBLIC "./src")

target_link_libraries(file-lib PUBLIC shared str-lib)
