# 307lib/TermAPI
cmake_minimum_required(VERSION 3.15)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# Create TermAPI project
project("TermAPI" VERSION ${307lib_VERSION} LANGUAGES CXX)

# Get headers & sources
file(GLOB HEADERS
	RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
	CONFIGURE_DEPENDS
	"include/*.h*"
)
file(GLOB SRC 
	RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
	CONFIGURE_DEPENDS
	"src/*.c*"
)

# Create static library
add_library(TermAPI STATIC ${SRC})

# Set library properties
set_property(TARGET TermAPI PROPERTY CXX_STANDARD 20)
set_property(TARGET TermAPI PROPERTY CXX_STANDARD_REQUIRED ON)
target_compile_options(TermAPI PUBLIC "/Zc:__cplusplus")

# make a list of absolute paths for headers
include(PrependEach)
PREPEND_EACH(HEADERS_ABS "${HEADERS}" "${CMAKE_CURRENT_SOURCE_DIR}")
# specify library headers
target_sources(TermAPI PUBLIC
	"$<BUILD_INTERFACE:${HEADERS_ABS}>"
	"$<INSTALL_INTERFACE:${HEADERS}>"
)

# Specify library include dirs
target_include_directories(TermAPI PUBLIC 
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include;${CMAKE_CURRENT_SOURCE_DIR}/src>"
	"$<INSTALL_INTERFACE:include;src>"
)

# Link library dependencies
target_link_libraries(TermAPI PUBLIC shared)

# Create library installation targets
if (307lib_ENABLE_PACKAGING)
	include(PackageInstaller)
	GENERATE_PACKAGING(TermAPI)
	WRITE_VERSION_FILE(TermAPI SameMajorVersion)
	INSTALL_PACKAGE(TermAPI)
endif()