# 307lib/optlib
cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
	# Fix MSVC __cplusplus macro
	string(APPEND CMAKE_CXX_FLAGS " /Zc:__cplusplus")
endif()

set(optlib_VERSION_MAJOR 1)
set(optlib_VERSION_MINOR 1)
set(optlib_VERSION_PATCH 0)
set(optlib_VERSION ${optlib_VERSION_MAJOR}.${optlib_VERSION_MINOR}.${optlib_VERSION_PATCH})

project("optlib" VERSION ${optlib_VERSION} LANGUAGES CXX)

set(HEADERS
	"include/Arg.hpp"
	"include/ArgContainer.hpp"
	"include/ArgParser.hpp"
	"include/VariantArgumentType.hpp"
	"include/ParamsAPI2.hpp"
)

include(CMakeDependentOption)
cmake_dependent_option(OPTLIB_ENABLE_EXPERIMENTAL "Enable the work-in-progress experimental ParamsAPI3 implementation. This is not complete or even working!" OFF "ENABLE_OPTLIB" OFF)
if (OPTLIB_ENABLE_EXPERIMENTAL)
	list(APPEND HEADERS "include/Argument.hpp")
endif()

file(GLOB SRC "src/*.cpp")

add_library(optlib STATIC ${HEADERS} ${SRC})

# include directories
target_include_directories(optlib PUBLIC 
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
	"$<INSTALL_INTERFACE:include>"
)

# link dependencies
target_link_libraries(optlib PUBLIC shared strlib)

if (307lib_ENABLE_PACKAGING) # Include packaging
	GENERATE_PACKAGING(optlib)
	WRITE_VERSION_FILE(optlib SameMajorVersion)
	INSTALL_PACKAGE(optlib)
endif()