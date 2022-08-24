########################################################################
#                                307lib                                #
########################################################################
cmake_minimum_required (VERSION 3.15)

# Append the 307modules directory to the CMAKE_MODULE_PATH list
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/307modules")

# Check if this is the top level directory.
string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" TOP_LEVEL)

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")		# MSVC / VC++
	set(307lib_compiler_commandline "/Zc:__cplusplus" "/Zc:preprocessor" "/permissive-" CACHE INTERNAL "Compiler commandline arguments for 307lib")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")		# gcc / g++ / xcode
	#set(307lib_compiler_commandline "-pedantic-errors" CACHE INTERNAL "Compiler commandline arguments for 307lib")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")	# clang
	if (CMAKE_CXX_COMPILER_FRONTEND_VARIANT MATCHES "MSVC")		# clang: MSVC / VC++
		set(307lib_compiler_commandline "/Zc:__cplusplus" "/Zc:preprocessor" "/permissive-" CACHE INTERNAL "Compiler commandline arguments for 307lib")
	elseif(CMAKE_CXX_COMPILER_FRONTEND_VARIANT MATCHES "GNU")	# clang: gcc / g++
		#set(307lib_compiler_commandline "-pedantic-errors" CACHE INTERNAL "Compiler commandline arguments for 307lib")
	endif()
endif()

if (BUILD_SHARED_LIBS)
	set(307lib_lib_type SHARED CACHE INTERNAL "")
else()
	set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
	set(307lib_lib_type STATIC CACHE INTERNAL "")
endif()

# Set a fallback version to use if the git tag couldn't be retrieved. (remote compiling, included as dependency, etc.)
set(ENV{307lib_VERSION} "0.0.0")

# Get the current version
include(VersionTag)
GET_VERSION_TAG("${CMAKE_CURRENT_SOURCE_DIR}" "307lib")

# create project
project("307lib" VERSION ${307lib_VERSION} LANGUAGES CXX)

# Setup packaging
option(307lib_ENABLE_PACKAGING "Enable library packaging" ${TOP_LEVEL})
if (${307lib_ENABLE_PACKAGING})
	include(PackageInstaller)
endif()
unset(TOP_LEVEL CACHE)

option(307lib_build_shared "Build the (307lib::shared) target." TRUE)
if (${307lib_build_shared})
	add_subdirectory("shared")
endif()

option(307lib_build_TermAPI "Build the (307lib::TermAPI) target." TRUE)
if (${307lib_build_TermAPI})
	add_subdirectory("TermAPI")
endif()

option(307lib_build_strlib "Build the (307lib::strlib) target." TRUE)
if (${307lib_build_strlib})
	add_subdirectory("strlib")
endif()

option(307lib_build_tokenlib "Build the (307lib::tokenlib) target." TRUE)
if (${307lib_build_tokenlib})
	add_subdirectory("tokenlib")
endif()

option(307lib_build_xlog "Build the (307lib::xlog) target." TRUE)
if (${307lib_build_xlog})
	add_subdirectory("xlog")
endif()

option(307lib_build_filelib "Build the (307lib::filelib) target." TRUE)
if (${307lib_build_filelib})
	add_subdirectory("filelib")
endif()

option(307lib_build_optlib "Build the (307lib::optlib) target." TRUE)
if (${307lib_build_optlib})
	add_subdirectory("optlib")
endif()

# Create an interface library
add_library(307lib INTERFACE)
# Set interface properties
set_property(TARGET 307lib PROPERTY CXX_STANDARD 20)
set_property(TARGET 307lib PROPERTY POSITION_INDEPENDENT_CODE ON)

# Enables the testing library.
option(307lib_build_testing "Enable the unstable development executable project in 307lib. This is not recommended." OFF)
if (${307lib_build_testing})
	add_subdirectory("testing")
endif()
