# 307lib/optlib
cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
	# Fix MSVC __cplusplus macro
	string(APPEND CMAKE_CXX_FLAGS " /Zc:__cplusplus")
endif()

set(optlib_VERSION_MAJOR 1)
set(optlib_VERSION_MINOR 0)
set(optlib_VERSION_PATCH 0)
set(optlib_VERSION ${optlib_VERSION_MAJOR}.${optlib_VERSION_MINOR}.${optlib_VERSION_PATCH})

project("optlib" VERSION ${optlib_VERSION})

set(HEADERS
	"./include/Arg.hpp"
	"./include/ArgContainer.hpp"
	"./include/ArgParser.hpp"
	"./include/VariantArgumentType.hpp"
	"./include/ParamsAPI2.hpp"
)
set(SRC
	"./src/ParamsAPI2.cpp"
)

add_library(optlib STATIC ${HEADERS} ${SRC})
add_library(307lib::optlib ALIAS optlib)
# link dependencies
target_link_libraries(optlib PUBLIC shared strlib)
# include directories
target_include_directories(optlib PUBLIC 
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
	"$<INSTALL_INTERFACE:include>"
)
target_link_directories(optlib PUBLIC 
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"
	"$<INSTALL_INTERFACE:src>"
)

# Packaging
include(GenerateExportHeader)
generate_export_header(optlib EXPORT_FILE_NAME "${CMAKE_CURRENT_SOURCE_DIR}/export.h")

if (307lib_ENABLE_PACKAGING)
	add_subdirectory("packaging")
endif()
