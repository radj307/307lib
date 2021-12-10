# UnitTests
cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
	# Fix MSVC __cplusplus macro
	string(APPEND CMAKE_CXX_FLAGS " /Zc:__cplusplus")
endif()

project("UnitTests")

include(FetchContent)
FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/054a986a8513149e8374fc669a5fe40117ca6b41.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(HEADERS
	"fixtures.hpp"
)

set(SOURCE
	"test.cpp"
)

set(TEST_ARGS
	
)

include(CTest)
include(GoogleTest)
add_executable(UnitTests ${HEADERS} ${SOURCE})

target_link_libraries(UnitTests PUBLIC gtest_main googletest shared str-lib TermAPI opt-lib file-lib)

enable_testing()
add_test(NAME vector_functions COMMAND UnitTests)
