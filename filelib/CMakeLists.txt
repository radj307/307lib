# 307lib/filelib
cmake_minimum_required(VERSION 3.15)

# Retrieve headers & sources
file(GLOB_RECURSE HEADERS
	RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
	CONFIGURE_DEPENDS
	"include/*.h*"
	"include/ini/*.h*"
)
file(GLOB_RECURSE SRCS
	RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
	CONFIGURE_DEPENDS
	"src/*.c*"
)

# Create static library
add_library(filelib ${307lib_lib_type} "${SRCS}")

# Set properties
set_property(TARGET filelib PROPERTY CXX_STANDARD 20)
set_property(TARGET filelib PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET filelib PROPERTY POSITION_INDEPENDENT_CODE ON)
set_property(TARGET filelib PROPERTY WINDOWS_EXPORT_ALL_SYMBOLS ON)

target_compile_options(filelib PRIVATE "${307lib_compiler_commandline}")

# Set library sources & include dirs
include(PrependEach)
PREPEND_EACH(HEADERS_ABS "${HEADERS}" "${CMAKE_CURRENT_SOURCE_DIR}/")

target_sources(filelib PUBLIC 
	"$<BUILD_INTERFACE:${HEADERS_ABS}>"
	"$<INSTALL_INTERFACE:${HEADERS}>"
)
target_include_directories(filelib PUBLIC 
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
	"$<INSTALL_INTERFACE:include;src>"
)

# Link library dependencies
target_link_libraries(filelib PUBLIC shared strlib tokenlib)

# Create library installation targets
if (307lib_ENABLE_PACKAGING) # Include packaging
	include(PackageInstaller)
	GENERATE_PACKAGING(filelib)
	WRITE_VERSION_FILE(filelib SameMajorVersion)
	INSTALL_PACKAGE(filelib)
endif()
