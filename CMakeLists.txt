# 307lib
cmake_minimum_required (VERSION 3.15)
#[[
To set options on the commandline, use -OPTION_NAME=OPTION_VALUE
 To set options in an inheriting CMakeLists.txt, use this line:

 set(OPTION_NAME OPTION_VALUE CACHE BOOL OPTION_DESCRIPTION)

| Option Name         | Default | Lib Dependencies | Description                                            |
| ------------------- | ------- | ---------------- | ------------------------------------------------------ |
| ENABLE_STRLIB       | ON      | shared           | Enable the string library.							    |
| ENABLE_TERMAPI      | ON      | shared           | Enable the TermAPI library.							|
| ENABLE_FILELIB      | ON      | shared strlib   | Enable the file/filesystem library.					|
| ENABLE_OPTLIB       | ON      | shared strlib   | Enable the commandline/environment library.			|
| ENABLE_TESTING      | OFF     | all              | Enable the debugging/testing executable.			    |
| ENABLE_UNITTESTS    | OFF     | variable         | Enable unit tests using the googletest framework.		|

| Sub-Option Name            | Project    | Default   | Description                                                                      |
| -------------------------- | ---------- | --------- | -------------------------------------------------------------------------------- |
| SHARED_DISABLE_WINDOWS_HPP | shared     | Dependent | Only available when building on windows, this disables the <Windows.hpp> header. |
| SHARED_ENABLE_XRAND        | shared     | ON        | Enables the <xRand.hpp> header.                                                  |
| TERMAPI_ENABLE_XLOG        | TermAPI v3 | ON        | Enables the <xLog.hpp> header.                                                   |
| FILELIB_ENABLE_EXTENSIONS  | filelib    | ON        | Includes all filelib components, including tokenization/parsing.                |
]]

set(CMAKE_POSITION_INDEPENDENT_CODE ON CACHE BOOL "" FORCE) # Enable position independent code for all subprojects
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/307modules")
# Get the current version
include(AutoVersion)
if (EXISTS "${CMAKE_SOURCE_DIR}/.git")
	message(STATUS "Git repository located, using AutoVersion::GET_VERSION...")
	GET_VERSION("${CMAKE_CURRENT_SOURCE_DIR}" 307lib_VERSION_MAJOR 307lib_VERSION_MINOR 307lib_VERSION_PATCH)
else()
	message(WARNING "No git repository found, using \"0.0.0\" as the version number by default.")
	set(307lib_VERSION_MAJOR "0" CACHE STRING "" FORCE)
	set(307lib_VERSION_MINOR "0" CACHE STRING "" FORCE)
	set(307lib_VERSION_PATCH "0" CACHE STRING "" FORCE)
endif()
MAKE_VERSION(307lib_VERSION ${307lib_VERSION_MAJOR} ${307lib_VERSION_MINOR} ${307lib_VERSION_PATCH})

# create project
project("307lib" VERSION ${307lib_VERSION} LANGUAGES CXX)

# Setup packaging
string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" TOP_LEVEL)

if (NOT DEFINED 307lib_VERSION OR "${307lib_VERSION}" STREQUAL "")
	set(TOP_LEVEL FALSE CACHE BOOL "" FORCE)
endif()

option(307lib_ENABLE_PACKAGING "Enable library packaging" ${TOP_LEVEL})
if (307lib_ENABLE_PACKAGING)
	include(PackageInstaller)
endif()
unset(TOP_LEVEL CACHE)

# Add shared lib
add_subdirectory("shared")

# Add TermAPI
option(ENABLE_TERMAPI "Enable the TermAPI library." ON)
if(ENABLE_TERMAPI)
	add_subdirectory("TermAPI")
endif()

# Add string library
option(ENABLE_STRLIB "Enable the string library." ON)
if (ENABLE_STRLIB)
	add_subdirectory("strlib")
endif()

# Add xlog
include(CMakeDependentOption)
cmake_dependent_option(ENABLE_XLOG "Enable building the xlog library." ON "ENABLE_STRLIB" OFF)
if (ENABLE_XLOG)
	add_subdirectory("xlog")
endif()

# Add file library
cmake_dependent_option(ENABLE_FILELIB "Enable the file library." ON "ENABLE_STRLIB" OFF)
if (ENABLE_FILELIB)
	add_subdirectory("filelib")
endif()


# Add opt library
cmake_dependent_option(ENABLE_OPTLIB "Enable the commandline option library." ON "ENABLE_STRLIB;ENABLE_FILELIB" OFF)
if(ENABLE_OPTLIB)
	add_subdirectory("optlib")
endif()

# Add testing executable
cmake_dependent_option(ENABLE_TESTING "Enable building an executable used for testing the project. This should be off when only using the library." OFF "ENABLE_STRLIB;ENABLE_OPTLIB;ENABLE_FILELIB;ENABLE_TERMAPI" OFF)
if (ENABLE_TESTING)
	add_subdirectory("testing")
endif()
