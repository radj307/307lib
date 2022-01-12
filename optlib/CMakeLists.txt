# 307lib/optlib
cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
	# Fix MSVC __cplusplus macro
	string(APPEND CMAKE_CXX_FLAGS " /Zc:__cplusplus")
endif()

project("optlib" VERSION ${307lib_VERSION} LANGUAGES CXX)

file(GLOB HEADERS
	RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
	CONFIGURE_DEPENDS
	"include/*.h*"
)

include(CMakeDependentOption)
cmake_dependent_option(OPTLIB_ENABLE_EXPERIMENTAL "Enable the work-in-progress experimental ParamsAPI3 implementation. This is not complete or even working!" OFF "ENABLE_OPTLIB" OFF)
if (OPTLIB_ENABLE_EXPERIMENTAL)
	list(APPEND HEADERS "include/Argument.hpp")
endif()

file(GLOB SRC
	RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
	CONFIGURE_DEPENDS
	"src/*.c*"
)

add_library(optlib STATIC ${SRC})

include(PrependEach)
PREPEND_EACH(HEADERS_ABS "${HEADERS}" "${CMAKE_CURRENT_SOURCE_DIR}")

target_sources(optlib PUBLIC
	"$<BUILD_INTERFACE:${HEADERS_ABS}>"
	"$<INSTALL_INTERFACE:${HEADERS}>"
)

# include directories
target_include_directories(optlib PUBLIC 
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
	"$<INSTALL_INTERFACE:include>"
)

# link dependencies
target_link_libraries(optlib PUBLIC shared strlib)

if (307lib_ENABLE_PACKAGING) # Include packaging
	GENERATE_PACKAGING(optlib)
	WRITE_VERSION_FILE(optlib SameMajorVersion)
	INSTALL_PACKAGE(optlib)
endif()