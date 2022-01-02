# 307lib/shared
cmake_minimum_required(VERSION 3.15)
include(CMakeDependentOption)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
	string(APPEND CMAKE_CXX_FLAGS " /Zc:__cplusplus") # Fix MSVC __cplusplus macro
endif()

set(shared_VERSION_MAJOR 1)
set(shared_VERSION_MINOR 1)
set(shared_VERSION_PATCH 0)
set(shared_VERSION ${shared_VERSION_MAJOR}.${shared_VERSION_MINOR}.${shared_VERSION_PATCH})

project("shared" VERSION ${shared_VERSION} LANGUAGES CXX)

file(GLOB SRCS 
	RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
	CONFIGURE_DEPENDS
	"include/*.h*"
	"src/*.c*"
)

add_library(shared STATIC ${SRCS})

target_include_directories(shared PUBLIC 
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include;${CMAKE_CURRENT_SOURCE_DIR}/src>"
	"$<INSTALL_INTERFACE:include;src>"
)

# Packaging
if (307lib_ENABLE_PACKAGING) # Include packaging
	GENERATE_PACKAGING(shared)
	WRITE_VERSION_FILE(shared SameMajorVersion)
	INSTALL_PACKAGE(shared)
endif()

