# TermAPI/v4/color
cmake_minimum_required(VERSION 3.15)

# require C++20
set(CMAKE_CXX_STANDARD 20)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
	# Fix MSVC __cplusplus macro
	string(APPEND CMAKE_CXX_FLAGS " /Zc:__cplusplus")
endif()

set(HEADERS
	"./include/color-values.h"
	"./include/color-transform.hpp"
	"./include/color-format.hpp"
	
	"./include/setcolor.hpp"
	
	"./include/palette.hpp"
)
set(SRC
	"./src/setcolor.cpp"
	"./src/palette.cpp"
)

add_library(color OBJECT ${HEADERS} ${SRC})
target_include_directories(color PUBLIC 
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
	"$<INSTALL_INTERFACE:include>"
)
target_link_directories(color PUBLIC 
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"
	"$<INSTALL_INTERFACE:src>"
)

target_link_libraries(color PUBLIC ANSI)
