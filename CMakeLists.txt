# 307lib
# To set options on the commandline, use -OPTION_NAME=OPTION_VALUE
# To set options in an inheriting CMakeLists.txt, use this line:
#
# set(OPTION_NAME OPTION_VALUE CACHE BOOL OPTION_DESCRIPTION)
#
# OPTION_VALUE = <ON|OFF>
# 
cmake_minimum_required (VERSION 3.13)
include(CMakeDependentOption)

# require C++20
set(CMAKE_CXX_STANDARD 20)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
	# Fix MSVC __cplusplus macro
	string(APPEND CMAKE_CXX_FLAGS " /Zc:__cplusplus")
endif()


# create project
project("307lib")

# Add shared lib
add_subdirectory("shared")

# Add string library
option(ENABLE_STRLIB "Enable the string library." ON)
if (ENABLE_STRLIB)
	add_subdirectory("str-lib")
endif()

# Add file library
cmake_dependent_option(ENABLE_FILELIB "Enable the file library." ON "ENABLE_STRLIB" OFF)
if (ENABLE_FILELIB)
	add_subdirectory("file-lib")
endif()

# Add opt library
cmake_dependent_option(ENABLE_OPTLIB "Enable the commandline option library." ON "ENABLE_STRLIB;ENABLE_FILELIB" OFF)
if(ENABLE_OPTLIB)
	add_subdirectory("opt-lib")
endif()

# Add TermAPI
cmake_dependent_option(ENABLE_TERMAPI "Enable the TermAPI library." ON "ENABLE_STRLIB" OFF)
if(ENABLE_TERMAPI)
	add_subdirectory("TermAPI")
endif()

# Add testing executable
cmake_dependent_option(ENABLE_TESTING "Enable building an executable used for testing the project. This should be off when only using the library." ON "ENABLE_STRLIB;ENABLE_OPTLIB;ENABLE_FILELIB;ENABLE_TERMAPI" OFF)
if (ENABLE_TESTING)
	add_subdirectory("testing")
endif()
