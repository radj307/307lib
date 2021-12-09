#### 307lib ####
# To set options on the commandline, use -OPTION_NAME=OPTION_VALUE
# To set options in an inheriting CMakeLists.txt, use this line:
#
# set(OPTION_NAME OPTION_VALUE CACHE BOOL OPTION_DESCRIPTION)
#
# OPTION_VALUE = <ON|OFF>
#
#### Options ####
# | Option Name			| Default | Dependencies            | Description                                           |
# | ------------------- | ------- | ----------------------- | ----------------------------------------------------- |
# | ENABLE_STRLIB		| ON	  | shared                  | Enable the string library.							|
# | ENABLE_TERMAPI		| ON	  | shared str-lib          | Enable the TermAPI library.							|
# | ENABLE_FILELIB		| ON	  | shared str-lib          | Enable the file/filesystem library.					|
# | ENABLE_OPTLIB		| ON	  | shared str-lib file-lib | Enable the commandline/environment library.			|
# | ENABLE_TESTING		| OFF	  | all                     | Enable the debugging/testing executable.			    |
# | ENABLE_UNITTESTS	| OFF	  | variable                | Enable unit tests using the googletest framework.		|
#
#### Sub-Options ####
# | Project  | Option Name                | Default  | Description                                                                      |
# | -------- | -------------------------- | -------- | -------------------------------------------------------------------------------- |
# | shared   | SHARED_DISABLE_WINDOWS_HPP | Variable | Only available when building on windows, this disables the <Windows.hpp> header. |
# | shared   | SHARED_ENABLE_XRAND        | ON       | Enables the <xRand.hpp> header.                                                  |
# | TermAPI  | TERMAPI_ENABLE_XLOG        | ON       | Enables the <xLog.hpp> header.                                                   |
# | file-lib | FILELIB_ENABLE_EXTENSIONS  | ON       | Includes all file-lib components, including tokenization/parsing.                |
#
cmake_minimum_required (VERSION 3.13)
include(CMakeDependentOption)

# require C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

# Add TermAPI
cmake_dependent_option(ENABLE_TERMAPI "Enable the TermAPI library." ON "ENABLE_STRLIB" OFF)
if(ENABLE_TERMAPI)
	add_subdirectory("TermAPI")
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

# Add testing executable
cmake_dependent_option(ENABLE_TESTING "Enable building an executable used for testing the project. This should be off when only using the library." OFF "ENABLE_STRLIB;ENABLE_OPTLIB;ENABLE_FILELIB;ENABLE_TERMAPI" OFF)
if (ENABLE_TESTING)
	add_subdirectory("testing")
endif()