# 307lib/tokenlib
cmake_minimum_required(VERSION 3.20)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# Create a project
project("tokenlib" VERSION ${307lib_VERSION} LANGUAGES CXX)

# Get a list of all headers
file(GLOB HEADERS
	RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
	CONFIGURE_DEPENDS
	"include/*.h*"
)
# Get a list of all sources
file(GLOB SRCS
	RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
	CONFIGURE_DEPENDS
	"src/*.c*"
)

# Create a static library
add_library(tokenlib STATIC "${SRCS}" "../cpp.hint")

# Set properties
set_property(TARGET tokenlib PROPERTY CXX_STANDARD 20)
set_property(TARGET tokenlib PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET tokenlib PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_features(tokenlib PUBLIC cxx_std_20)
if (MSVC)
	target_compile_options(tokenlib PUBLIC "/Zc:__cplusplus" "/Zc:preprocessor")
endif()

# Include headers
include(PrependEach)
PREPEND_EACH(HEADERS_ABS "${HEADERS}" "${CMAKE_CURRENT_SOURCE_DIR}/")
target_sources(tokenlib PUBLIC
	"$<BUILD_INTERFACE:${HEADERS_ABS}>"
	"$<INSTALL_INTERFACE:${HEADERS}>"
)

# Include directories
target_include_directories(tokenlib PUBLIC
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
	"$<INSTALL_INTERFACE:include>"
)

# Link library dependencies
target_link_libraries(tokenlib PUBLIC shared strlib)

# Create library installation targets
if (307lib_ENABLE_PACKAGING) # Include packaging
	include(PackageInstaller)
	GENERATE_PACKAGING(tokenlib)
	WRITE_VERSION_FILE(tokenlib SameMajorVersion)
	INSTALL_PACKAGE(tokenlib)
endif()
