﻿########################################################################
#                                307lib                                #
########################################################################
cmake_minimum_required (VERSION 3.15)

# Append the 307modules directory to the CMAKE_MODULE_PATH list
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/307modules")

# Check if this is the top level directory.
string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" TOP_LEVEL)


# Set a fallback version to use if the git tag couldn't be retrieved. (remote compiling, included as dependency, etc.)
set(ENV{307lib_VERSION} "0.0.0")

# Get the current version
include(VersionTag)
GET_VERSION_TAG("${CMAKE_CURRENT_SOURCE_DIR}" "307lib")

# create project
project("307lib" VERSION ${307lib_VERSION} LANGUAGES CXX)

# Setup packaging
option(307lib_ENABLE_PACKAGING "Enable library packaging" ${TOP_LEVEL})
if (307lib_ENABLE_PACKAGING)
	include(PackageInstaller)
endif()
unset(TOP_LEVEL CACHE)

add_subdirectory("shared")

add_subdirectory("TermAPI")

add_subdirectory("strlib")

add_subdirectory("tokenlib")

add_subdirectory("xlog")

add_subdirectory("filelib")

add_subdirectory("optlib")

# Create an interface library
add_library(307lib INTERFACE)

option(307lib_ENABLE_DEV_PROJECT "Enable the unstable development executable project in 307lib. This is not recommended." OFF)
if (${307lib_ENABLE_DEV_PROJECT})
	add_subdirectory("testing")
endif()
