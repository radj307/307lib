# TermAPI/extensions
cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
	# Fix MSVC __cplusplus macro
	string(APPEND CMAKE_CXX_FLAGS " /Zc:__cplusplus")
endif()

project("TermAPI Extensions")

set(HEADERS
	"./include/xlog.hpp"
	"./include/color-config.hpp"
)
set(SRC
	"./src/xlog.cpp"
	"./src/color-config.cpp"
)

add_library(extensions OBJECT ${HEADERS} ${SRC})
target_include_directories(extensions PUBLIC 
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
	"$<INSTALL_INTERFACE:include>"
)
target_link_directories(extensions PUBLIC 
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"
	"$<INSTALL_INTERFACE:src>"
)

target_link_libraries(extensions PUBLIC ANSI color term str-lib file-lib)
