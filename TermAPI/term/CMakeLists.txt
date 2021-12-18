# TermAPI/v4/term
cmake_minimum_required(VERSION 3.15)

# require C++20
set(CMAKE_CXX_STANDARD 20)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
	# Fix MSVC __cplusplus macro
	string(APPEND CMAKE_CXX_FLAGS " /Zc:__cplusplus")
endif()

set(HEADERS
	"./include/CursorOrigin.h"
	"./include/term.hpp"
)
set(SRC
	"./src/term.cpp"
)
option(TERMAPI_ENABLE_MESSAGE "Enable the Message.hpp header." ON)

if (TERMAPI_ENABLE_MESSAGE)
	list(APPEND HEADERS "./include/Message.hpp")
endif()

add_library(term OBJECT ${HEADERS} ${SRC})
target_include_directories(term PUBLIC 
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
	"$<INSTALL_INTERFACE:include>"
)
target_link_directories(term PUBLIC 
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"
	"$<INSTALL_INTERFACE:src>"
)

target_link_libraries(term PUBLIC ANSI)

if (TERMAPI_ENABLE_XLOG)
	target_link_libraries(term PUBLIC str-lib)
endif()
