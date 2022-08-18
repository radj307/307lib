# 307lib/strlib
cmake_minimum_required (VERSION 3.13)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

project("strlib" VERSION ${307lib_VERSION} LANGUAGES CXX)

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

add_library(strlib STATIC "${SRCS}")

set_property(TARGET strlib PROPERTY CXX_STANDARD 20)
set_property(TARGET strlib PROPERTY CXX_STANDARD_REQUIRED OFF)
set_property(TARGET strlib PROPERTY POSITION_INDEPENDENT_CODE ON)

target_compile_options(strlib PRIVATE "${307lib_compiler_commandline}")

include(PrependEach)
PREPEND_EACH(HEADERS_ABS "${HEADERS}" "${CMAKE_CURRENT_SOURCE_DIR}/")

target_sources(strlib PUBLIC 
	"$<BUILD_INTERFACE:${HEADERS_ABS}>"
	"$<INSTALL_INTERFACE:${HEADERS}>"
)

target_include_directories(strlib PUBLIC 
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
	"$<INSTALL_INTERFACE:include>"
)

target_link_libraries(strlib PUBLIC shared)

# Packaging
if (307lib_ENABLE_PACKAGING) # Include packaging
	include(PackageInstaller)
	GENERATE_PACKAGING(strlib)
	WRITE_VERSION_FILE(strlib SameMajorVersion)
	INSTALL_PACKAGE(strlib)
endif()
