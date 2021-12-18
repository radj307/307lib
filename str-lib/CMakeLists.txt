# str-lib
cmake_minimum_required (VERSION 3.13)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
	# Fix MSVC __cplusplus macro
	string(APPEND CMAKE_CXX_FLAGS " /Zc:__cplusplus")
endif()

project("str-lib")

set(HEADERS
	"./include/strconv.hpp"
	"./include/strmanip.hpp"
	"./include/strlocale.hpp"
	"./include/str.hpp"
	"./include/strutility.hpp"
)
set(SRC
	"./src/str.cpp"
)

add_library(str-lib STATIC ${HEADERS} ${SRC})
target_link_libraries(str-lib PUBLIC shared)
target_include_directories(str-lib PUBLIC 
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
	"$<INSTALL_INTERFACE:include>"
)
target_link_directories(str-lib PUBLIC 
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"
	"$<INSTALL_INTERFACE:src>"
)
#set_target_properties(str-lib PROPERTIES LINKER_LANGUAGE CXX)

# Packaging
include(GenerateExportHeader)
generate_export_header(str-lib EXPORT_FILE_NAME "${CMAKE_CURRENT_SOURCE_DIR}/export.h")

# Include packaging by default if this is the top-level directory
if (307lib_ENABLE_PACKAGING)
	add_subdirectory("packaging")
endif()

