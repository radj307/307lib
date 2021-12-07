cmake_minimum_required (VERSION 3.13)

# require C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
	# Fix MSVC __cplusplus macro
	string(APPEND CMAKE_CXX_FLAGS " /Zc:__cplusplus")
endif()


# create project
project("cpp-307lib")

# Add shared libs
add_subdirectory("shared")

# Add str-lib library
add_subdirectory("str-lib")

option(ENABLE_TESTING "Enable building an executable used for testing the project" ON)

# Add testing executable
if (ENABLE_TESTING)
	add_subdirectory("testing")
endif()
