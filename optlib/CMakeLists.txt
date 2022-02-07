# 307lib/optlib
cmake_minimum_required(VERSION 3.13)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# Create project
project("optlib" VERSION ${307lib_VERSION} LANGUAGES CXX)

# Get headers
file(GLOB HEADERS
	RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
	CONFIGURE_DEPENDS
	"include/*.h*"
)

# Check experimental option
include(CMakeDependentOption)
cmake_dependent_option(OPTLIB_ENABLE_EXPERIMENTAL "Enable the work-in-progress experimental ParamsAPI3 implementation. This is not complete or even working!" OFF "ENABLE_OPTLIB" OFF)
if (OPTLIB_ENABLE_EXPERIMENTAL)
	list(APPEND HEADERS "include/Argument.hpp")
endif()

# Get sources
file(GLOB SRC
	RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
	CONFIGURE_DEPENDS
	"src/*.c*"
)

# Create static library
add_library(optlib STATIC ${SRC})

# Set library properties
set_property(TARGET optlib PROPERTY CXX_STANDARD 20)
set_property(TARGET optlib PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET optlib PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_features(optlib PUBLIC cxx_std_20)
if (MSVC)
	target_compile_options(optlib PUBLIC "/Zc:__cplusplus")
endif()

# Add headers
include(PrependEach)
PREPEND_EACH(HEADERS_ABS "${HEADERS}" "${CMAKE_CURRENT_SOURCE_DIR}/")
target_sources(optlib PUBLIC
	"$<BUILD_INTERFACE:${HEADERS_ABS}>"
	"$<INSTALL_INTERFACE:${HEADERS}>"
)

# add include directories
target_include_directories(optlib PUBLIC 
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
	"$<INSTALL_INTERFACE:include>"
)

# link optlib dependencies
target_link_libraries(optlib PUBLIC strlib tokenlib)

# Create installation targets
if (307lib_ENABLE_PACKAGING) # Include packaging
	include(PackageInstaller)
	GENERATE_PACKAGING(optlib)
	WRITE_VERSION_FILE(optlib SameMajorVersion)
	INSTALL_PACKAGE(optlib)
endif()