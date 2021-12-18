# TermAPI
cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
	# Fix MSVC __cplusplus macro
	string(APPEND CMAKE_CXX_FLAGS " /Zc:__cplusplus")
endif()

project("TermAPI")

# Add sequence library
add_subdirectory("ANSI")
		
set(HEADERS
	"./include/TermAPI.hpp"
)
set(SRC
	"./src/TermAPI.cpp"
)

add_library(TermAPI STATIC ${HEADERS} ${SRC})
add_library(307lib::TermAPI ALIAS TermAPI)
target_include_directories(TermAPI PUBLIC 
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
	"$<INSTALL_INTERFACE:include>"
)
target_link_directories(TermAPI PUBLIC 
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"
	"$<INSTALL_INTERFACE:src>"
)

set(LINKS
	"ANSI"
)

# Add color
option(TERMAPI_ENABLE_COLOR "Enable the color object library." ON)
if (TERMAPI_ENABLE_COLOR)
	add_subdirectory("color")
	list(APPEND LINKS
		"color"
	)
endif()

# Add term
option(TERMAPI_ENABLE_TERM "Enable the term object library." ON)
if (TERMAPI_ENABLE_TERM)
	add_subdirectory("term")
	list(APPEND LINKS
		"term"
	)
endif()

target_link_libraries(TermAPI PUBLIC ${LINKS})

include(CMakeDependentOption)
cmake_dependent_option(TERMAPI_ENABLE_EXTENSIONS "Enable the TermAPI extensions library" ON "ENABLE_STRLIB;ENABLE_FILELIB" OFF)
if(TERMAPI_ENABLE_EXTENSIONS)
	message(STATUS "TermAPI Extensions Enabled")
	add_subdirectory("extensions")
	target_link_libraries(TermAPI PUBLIC file-lib extensions)
endif()

# Packaging
include(GenerateExportHeader)
generate_export_header(TermAPI EXPORT_FILE_NAME "${CMAKE_CURRENT_SOURCE_DIR}/export.h")

# Include packaging by default if this is the top-level directory
if (307lib_ENABLE_PACKAGING)
	add_subdirectory("packaging")
endif()
