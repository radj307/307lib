# 307lib/filelib
cmake_minimum_required(VERSION 3.15)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# Create project
project("filelib" VERSION ${307lib_VERSION} LANGUAGES CXX)

# Retrieve headers & sources
file(GLOB HEADERS
	RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
	CONFIGURE_DEPENDS
	"include/*.h*"
)
file(GLOB SRCS
	RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
	CONFIGURE_DEPENDS
	"src/*.c*"
)

# Create static library
add_library(filelib STATIC ${SRCS})

set_property(TARGET filelib PROPERTY CXX_STANDARD 20)
set_property(TARGET filelib PROPERTY CXX_STANDARD_REQUIRED ON)
if (MSVC)
	target_compile_options(filelib PUBLIC "/Zc:__cplusplus")
endif()

# Set library properties
#set_property(TARGET filelib PROPERTY CXX_STANDARD 20)

#include(InitProperties)
#INIT_TARGET_PROPERTIES(filelib 20 REQUIRED)

# Set library sources & include dirs
include(PrependEach)
PREPEND_EACH(HEADERS_ABSOLUTE ${HEADERS} "${CMAKE_CURRENT_SOURCE_DIR}/")
target_sources(filelib PUBLIC 
	"$<BUILD_INTERFACE:${HEADERS_ABSOLUTE}>"
	"$<INSTALL_INTERFACE:${HEADERS}>"
)
target_include_directories(filelib PUBLIC 
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
	"$<INSTALL_INTERFACE:include;src>"
)

# Link library dependencies
target_link_libraries(filelib PUBLIC shared strlib)

# Create library installation targets
if (307lib_ENABLE_PACKAGING) # Include packaging
	include(PackageInstaller)
	GENERATE_PACKAGING(filelib)
	WRITE_VERSION_FILE(filelib SameMajorVersion)
	INSTALL_PACKAGE(filelib)
endif()