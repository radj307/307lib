# 307lib/testing/UnitTests/tests/ArgumentRedux_Test.cpp
cmake_minimum_required(VERSION 3.20)

set(TEST_NAME "ArgumentRedux_Test")


# Configure general properties
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# Create a unit test executable
add_executable("${TEST_NAME}" "${TEST_NAME}.cpp")

# Set properties
set_property(TARGET "${TEST_NAME}" PROPERTY CXX_STANDARD 20)
set_property(TARGET "${TEST_NAME}" PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET "${TEST_NAME}" PROPERTY POSITION_INDEPENDENT_CODE ON)
if (MSVC)
	target_compile_options("${TEST_NAME}" PRIVATE "/Zc:__cplusplus")
endif()

target_link_libraries("${TEST_NAME}" PRIVATE optlib)

# Use googletest to discover tests
include(GoogleTest)
GTEST_DISCOVER_TESTS(
	"${TEST_NAME}" #< target
	WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)
