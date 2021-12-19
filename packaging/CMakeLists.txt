# 307lib/packaging
# Uses configure_file() to create packaging files for libraries
cmake_minimum_required(VERSION 3.15)

if (POLICY CMP0074)
	cmake_policy(SET CMP0074 NEW)
endif()

function(setup_packaging _name)
	unset(PACKAGING_TARGET_NAME CACHE)
	# Set the cache variable that will be replaced in the copied file
	set(PACKAGING_TARGET_NAME "${_name}" CACHE STRING "Cache variable used by the packaging handler to determine the current target" FORCE)
	# create the packaging directory if it doesn't exist
	file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/${_name}/packaging")
	# copy the packaging file to the target
	configure_file("${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt.in" "${CMAKE_SOURCE_DIR}/${_name}/packaging/CMakeLists.txt" @ONLY)
	# copy the Config.cmake file to the target
	configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config.cmake.in" "${CMAKE_SOURCE_DIR}/${_name}/packaging/${_name}-config.cmake" @ONLY)
	# Set the find_package search directory
	set(${_name}_DIR "${CMAKE_SOURCE_DIR}/${_name}/packaging" CACHE STRING "Set the search directory for find_package" FORCE)
endfunction()

# Setup packaging for shared
message(STATUS "Generating packaging information for shared")
setup_packaging("shared")

# Setup packaging for str-lib
if (ENABLE_STRLIB)
	message(STATUS "Generating packaging information for str-lib")
	setup_packaging("str-lib")
endif()

# Setup packaging for file-lib
if (ENABLE_FILELIB)
	message(STATUS "Generating packaging information for file-lib")
	setup_packaging("file-lib")
endif()

# Setup packaging for TermAPI
if (ENABLE_TERMAPI)
	message(STATUS "Generating packaging information for TermAPI")
	setup_packaging("TermAPI")
endif()

# Setup packaging for opt-lib
if (ENABLE_OPTLIB)
	message(STATUS "Generating packaging information for opt-lib")
	setup_packaging("opt-lib")
endif()

# Cleanup
unset(PACKAGING_TARGETS CACHE)
unset(PACKAGING_TARGET_NAME CACHE)
