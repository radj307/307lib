# 307lib/strlib
cmake_minimum_required (VERSION 3.13)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
	# Fix MSVC __cplusplus macro
	string(APPEND CMAKE_CXX_FLAGS " /Zc:__cplusplus")
endif()

project("strlib" VERSION ${307lib_VERSION} LANGUAGES CXX)

set(HEADERS
	"include/strconv.hpp"
	"include/strmanip.hpp"
	"include/strlocale.hpp"
	"include/str.hpp"
	"include/strutility.hpp"
)
file(GLOB SRC "src/*.cpp")

add_library(strlib STATIC ${HEADERS} ${SRC})

target_include_directories(strlib PUBLIC 
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
	"$<INSTALL_INTERFACE:include>"
)

target_link_libraries(strlib PUBLIC shared)

# Packaging
if (307lib_ENABLE_PACKAGING) # Include packaging
	GENERATE_PACKAGING(strlib)
	WRITE_VERSION_FILE(strlib SameMajorVersion)
	INSTALL_PACKAGE(strlib)
endif()
