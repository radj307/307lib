# 307lib/packaging
# Uses configure_file() to create packaging files for libraries
cmake_minimum_required(VERSION 3.15)

if (POLICY CMP0074)
	cmake_policy(SET CMP0074 NEW)
endif()

function(setup_packaging _name _targets)
	# Set the cache variable that will be replaced in the copied file
	set(PACKAGING_TARGET_NAME "${_name}" CACHE STRING "Cache variable used by the packaging handler to determine the current target" FORCE)
	set(PACKAGING_TARGETS "${_targets}" CACHE STRING "Cache variable used by the packaging handler to determine the current target" FORCE)
	# create the packaging directory if it doesn't exist
	file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/${_name}/packaging")
	# copy the packaging file to the target
	configure_file("${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt.in" "${CMAKE_SOURCE_DIR}/${_name}/packaging/CMakeLists.txt" @ONLY)
	# copy the Config.cmake file to the target
	configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config.cmake.in" "${CMAKE_SOURCE_DIR}/${_name}/packaging/${_name}-config.cmake" @ONLY)
	# Set the find_package search directory
	set(${_name}_DIR "${CMAKE_SOURCE_DIR}/${_name}/packaging" CACHE STRING "Set the search directory for find_package" FORCE)
endfunction()

setup_packaging("shared" "shared")

if (ENABLE_STRLIB)
	setup_packaging("str-lib" "str-lib")
endif()

if (ENABLE_FILELIB)
	setup_packaging("file-lib" "file-lib")
endif()

if (ENABLE_TERMAPI)
	setup_packaging("TermAPI" "TermAPI")
endif()

if (ENABLE_OPTLIB)
	setup_packaging("opt-lib" "opt-lib")
endif()
