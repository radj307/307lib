# TermAPI
# (version selector)
cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
	# Fix MSVC __cplusplus macro
	string(APPEND CMAKE_CXX_FLAGS " /Zc:__cplusplus")
endif()

project("TermAPI")

# Enable deprecation warning if it wasn't already set
set(CMAKE_WARN_DEPRECATED ON)

# Set default version if it isn't already set
if (TERMAPI_VERSION_3)
	message(DEPRECATION "The set(TERMAPI_VERSION_3 ON) option has been deprecated. Use set(TERMAPI_VERSION <version>) instead!")
	set(TERMAPI_VERSION 3)
else()
	set(TERMAPI_VERSION 4)
endif()


if (TERMAPI_VERSION EQUAL 4)			# v4
	message(STATUS "Using TermAPI v4")
	add_subdirectory("v4")
elseif(TERMAPI_VERSION LESS_EQUAL 3)	# v3
	message(STATUS "Using TermAPI v3")
	add_subdirectory("v3")
else()
	message(WARNING "TERMAPI_VERSION is invalid; Version ${TERMAPI_VERSION} doesn't exist.")
endif()