# testing
cmake_minimum_required (VERSION 3.13)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
	# Fix MSVC __cplusplus macro
	string(APPEND CMAKE_CXX_FLAGS " /Zc:__cplusplus")
endif()

project("testing")

set(HEADERS
	"flagset.hpp"
	"performance.hpp"
)

set(SRC
	"main.cpp"
)

add_executable(testing ${HEADERS} ${SRC})
target_link_libraries(testing PUBLIC shared str-lib TermAPI opt-lib file-lib)

