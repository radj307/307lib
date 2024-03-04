# 307lib/shared

# Get headers & sources
file(GLOB_RECURSE HEADERS
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
add_library(shared ${307lib_lib_type} "${SRCS}" "cpp.hint")
add_library(307lib::shared ALIAS shared)

# Set library properties
set_property(TARGET shared PROPERTY CXX_STANDARD 20)
set_property(TARGET shared PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET shared PROPERTY POSITION_INDEPENDENT_CODE ON)
set_property(TARGET shared PROPERTY WINDOWS_EXPORT_ALL_SYMBOLS ON)

target_compile_options(shared PRIVATE "${307lib_compiler_commandline}")

include(PrependEach)
PREPEND_EACH(HEADERS_ABS "${HEADERS}" "${CMAKE_CURRENT_SOURCE_DIR}/")

target_sources(shared PUBLIC 
	"$<BUILD_INTERFACE:${HEADERS_ABS}>"
	"$<INSTALL_INTERFACE:${HEADERS}>"
)

target_include_directories(shared PUBLIC 
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include;${CMAKE_CURRENT_SOURCE_DIR}/src>"
	"$<INSTALL_INTERFACE:include;src>"
)

# Use CMake for preprocessor compiler detection

# Allow "AppleClang" for CMAKE_CXX_COMPILER_ID (https://cmake.org/cmake/help/latest/policy/CMP0025.html)
cmake_policy(SET CMP0025 NEW)

message("CMAKE_CXX_COMPILER_ID:      ${CMAKE_CXX_COMPILER_ID}")
message("CMAKE_CXX_COMPILER_VERSION: ${CMAKE_CXX_COMPILER_VERSION}")
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	target_compile_definitions(shared PUBLIC COMPILER_CLANG)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
	target_compile_definitions(shared PUBLIC COMPILER_APPLECLANG)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_definitions(shared PUBLIC COMPILER_GCC)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
	target_compile_definitions(shared PUBLIC COMPILER_INTEL)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	target_compile_definitions(shared PUBLIC COMPILER_MSVC)
endif()
target_compile_definitions(shared PUBLIC "COMPILER_ID=${CMAKE_CXX_COMPILER_ID}")

# Packaging
if (307lib_ENABLE_PACKAGING) # Include packaging
	include(PackageInstaller)
	GENERATE_PACKAGING(shared)
	WRITE_VERSION_FILE(shared SameMajorVersion)
	INSTALL_PACKAGE(shared)
endif()

