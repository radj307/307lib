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
| FILELIB_ENABLE_EXTENSIONS  | filelib   | ON        | Includes all filelib components, including tokenization/parsing.                |
]]

set(CMAKE_POSITION_INDEPENDENT_CODE ON) # Enable position independent code for all subprojects

# require C++20
set(CMAKE_CXX_STANDARD 20)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
	# Fix MSVC __cplusplus macro
	string(APPEND CMAKE_CXX_FLAGS " /Zc:__cplusplus")
else()
	set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

# Setup packaging
string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" TOP_LEVEL)
option(307lib_ENABLE_PACKAGING "Enable library packaging" "${TOP_LEVEL}")
if (307lib_ENABLE_PACKAGING)
	list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")
	set(ENABLE_INSTALL_MODULES FALSE CACHE BOOL "When true the library's cmake modules are included in the install directory.")
	if (ENABLE_INSTALL_MODULES)
		# TODO
	endif()
	include(PackageInstaller)
endif()

set(ENABLE_GIT_AUTOVERSION FALSE CACHE BOOL "Enable auto-versioning using git tags.")
if (ENABLE_GIT_AUTOVERSION)
	include(AutoVersion)
	PARSE_VERSION_STRING(${CMAKE_SOURCE_DIR} 307lib_VERSION_MAJOR 307lib_VERSION_MINOR 307lib_VERSION_PATCH 307lib_VERSION_EXTRA)
	CONCAT_VERSION_STRING(307lib_VERSION ${307lib_VERSION_MAJOR} ${307lib_VERSION_MINOR} ${307lib_VERSION_PATCH})
	set(307lib_VERSION ${307lib_VERSION_MAJOR}.${307lib_VERSION_MINOR}.${307lib_VERSION_PATCH})
else()
	set(307lib_VERSION "0.0.0")
endif()

# create project
project("307lib" VERSION ${307lib_VERSION} LANGUAGES CXX)

# Add shared lib
add_subdirectory("shared")

# Add string library
option(ENABLE_STRLIB "Enable the string library." ON)
if (ENABLE_STRLIB)
	add_subdirectory("strlib")
endif()

# Add file library
cmake_dependent_option(ENABLE_FILELIB "Enable the file library." ON "ENABLE_STRLIB" OFF)
if (ENABLE_FILELIB)
	add_subdirectory("filelib")
endif()

# Add TermAPI
option(ENABLE_TERMAPI "Enable the TermAPI library." ON)
if(ENABLE_TERMAPI)
	add_subdirectory("TermAPI")
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
