# 307lib/opt-lib
cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
	# Fix MSVC __cplusplus macro
	string(APPEND CMAKE_CXX_FLAGS " /Zc:__cplusplus")
endif()

project("opt-lib")

set(HEADERS
	"./include/Arg.hpp"
	"./include/ArgContainer.hpp"
	"./include/ArgParser.hpp"
	"./include/VariantArgumentType.hpp"
	"./include/ParamsAPI2.hpp"
)
set(SRC
	"./src/ParamsAPI2.cpp"
)

add_library(opt-lib STATIC ${HEADERS} ${SRC})
add_library(307lib::opt-lib ALIAS opt-lib)
# link dependencies
target_link_libraries(opt-lib PUBLIC shared str-lib)
# include directories
target_include_directories(opt-lib PUBLIC 
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
	"$<INSTALL_INTERFACE:include>"
)
target_link_directories(opt-lib PUBLIC 
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"
	"$<INSTALL_INTERFACE:src>"
)

# Packaging
include(GenerateExportHeader)
generate_export_header(opt-lib EXPORT_FILE_NAME "${CMAKE_CURRENT_SOURCE_DIR}/export.h")

if (307lib_ENABLE_PACKAGING)
	add_subdirectory("packaging")
endif()
