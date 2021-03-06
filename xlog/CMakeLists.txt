# 307lib/xlog
cmake_minimum_required(VERSION 3.15)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# Create project
project(xlog VERSION ${307lib_VERSION} LANGUAGES CXX)

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
add_library(xlog STATIC "${SRCS}" "../cpp.hint")

# Set library properties
set_property(TARGET xlog PROPERTY CXX_STANDARD 20)
set_property(TARGET xlog PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET xlog PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_features(xlog PUBLIC cxx_std_20)
if (MSVC)
	target_compile_options(xlog PUBLIC "/Zc:__cplusplus" "/Zc:preprocessor")
endif()

# Set library headers
include (PrependEach)
PREPEND_EACH(HEADERS_ABS "${HEADERS}" "${CMAKE_CURRENT_SOURCE_DIR}/")
target_sources(xlog PUBLIC
	"$<BUILD_INTERFACE:${HEADERS_ABS}>"
	"$<INSTALL_INTERFACE:${HEADERS}>"
)

# Set library include directories
target_include_directories(xlog PUBLIC
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include;${CMAKE_CURRENT_SOURCE_DIR}/src>"
	"$<INSTALL_INTERFACE:include;src>"
)

# Link library dependencies
target_link_libraries(xlog PUBLIC TermAPI strlib filelib)

# Create library installation targets
if (307lib_ENABLE_PACKAGING) # Include packaging
	include(PackageInstaller)
	GENERATE_PACKAGING(xlog)
	WRITE_VERSION_FILE(xlog SameMajorVersion)
	INSTALL_PACKAGE(xlog)
endif()
