# TermAPI/v4/ANSI
cmake_minimum_required(VERSION 3.15)

# require C++20
set(CMAKE_CXX_STANDARD 20)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
	# Fix MSVC __cplusplus macro
	string(APPEND CMAKE_CXX_FLAGS " /Zc:__cplusplus")
endif()

set(HEADERS
	"./include/Segments.h"
	"./include/Sequence.hpp"
	"./include/winEnableANSI.hpp"
	"./include/LineCharacter.hpp"
)
set(SRC
	"./src/Sequence.cpp"
)

add_library(ANSI OBJECT ${HEADERS} ${SRC})
target_include_directories(ANSI PUBLIC "./include" "./src")
target_link_directories(ANSI PUBLIC "./src")

target_link_libraries(ANSI PUBLIC shared)

