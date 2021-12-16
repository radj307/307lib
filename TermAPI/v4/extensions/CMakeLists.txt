cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
	# Fix MSVC __cplusplus macro
	string(APPEND CMAKE_CXX_FLAGS " /Zc:__cplusplus")
endif()

project("TermAPI Extensions")

set(HEADERS
	"./include/color-config.hpp"
)
set(SRC
	"./src/color-config.cpp"
)

add_library(extensions OBJECT ${HEADERS} ${SRC})
target_include_directories(extensions PUBLIC "./include" "./src")
target_link_directories(extensions PUBLIC "./src")

target_link_libraries(extensions PUBLIC ANSI color file-lib)
