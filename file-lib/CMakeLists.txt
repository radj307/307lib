# file-lib
cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
	# Fix MSVC __cplusplus macro
	string(APPEND CMAKE_CXX_FLAGS " /Zc:__cplusplus")
endif()

project("file-lib")

option(FILELIB_ENABLE_EXTENSIONS "Enable extensions of the file-lib library, including tokenization/parsing capabilities & more. If this is disabled, only fileio & fileutil will be available." OFF)

set(HEADERS
	"include/fileio.hpp"
	"include/fileutil.hpp"
)
set(SOURCE
	"src/fileio.cpp"
	"src/fileutil.cpp"
)
if(FILELIB_ENABLE_EXTENSIONS)
	set(HEADERS ${HEADERS} "include/")
	set(SOURCE ${SOURCE} "src/")
endif()

add_library(file-lib STATIC ${HEADERS} ${SOURCE})

target_link_libraries(file-lib PUBLIC shared str-lib)

target_include_directories(file-lib PUBLIC "./include" "./src")