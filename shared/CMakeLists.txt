# 307lib/shared
cmake_minimum_required(VERSION 3.15)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# Create shared project
project("shared" VERSION ${307lib_VERSION} LANGUAGES CXX)

# Get headers & sources
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
add_library(shared STATIC "${SRCS}" "../cpp.hint")

# Set library properties
set_property(TARGET shared PROPERTY CXX_STANDARD 20)
set_property(TARGET shared PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET shared PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_features(shared PUBLIC cxx_std_20)
if (MSVC)
	target_compile_options(shared PUBLIC "/Zc:__cplusplus" "/Zc:preprocessor")
endif()

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

# Packaging
if (307lib_ENABLE_PACKAGING) # Include packaging
	include(PackageInstaller)
	GENERATE_PACKAGING(shared)
	WRITE_VERSION_FILE(shared SameMajorVersion)
	INSTALL_PACKAGE(shared)
endif()

