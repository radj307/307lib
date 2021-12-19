# 307lib/packaging
# Uses configure_file() to create packaging files for libraries
cmake_minimum_required(VERSION 3.15)

if (POLICY CMP0074) # Set find_package() behavior to check the value of {<PACKAGE>_ROOT} variable.
	cmake_policy(SET CMP0074 NEW)
endif()

function(setup_lib_packaging _name)
	unset(PACKAGING_TARGET_NAME CACHE)
	# Set the cache variable that will be replaced in the copied file
	set(PACKAGING_TARGET_NAME "${_name}" CACHE STRING "Cache variable used by the packaging handler to determine the current target" FORCE)

	set(PACKAGING_TARGET_DIR "${CMAKE_SOURCE_DIR}/${_name}/packaging")
	file(REMOVE_RECURSE ${PACKAGING_TARGET_DIR}) # Delete the packaging directory recursively
	file(MAKE_DIRECTORY ${PACKAGING_TARGET_DIR}) # Re-Create the packaging directory
	configure_file("${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt.in" "${CMAKE_SOURCE_DIR}/${_name}/packaging/CMakeLists.txt" @ONLY) # copy the packaging/CMakeLists.txt file to the target
	configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config.cmake.in" "${CMAKE_SOURCE_DIR}/${_name}/packaging/${_name}-config.cmake" @ONLY) # copy the packaging/<name>-config.cmake file to the target
endfunction()

# Setup packaging for shared
message(STATUS "Generating packaging information for shared")
setup_lib_packaging("shared")

# Setup packaging for strlib
if (ENABLE_STRLIB)
	message(STATUS "Generating packaging information for strlib")
	setup_lib_packaging("strlib")
endif()

# Setup packaging for filelib
if (ENABLE_FILELIB)
	message(STATUS "Generating packaging information for filelib")
	setup_lib_packaging("filelib")
endif()

# Setup packaging for TermAPI
if (ENABLE_TERMAPI)
	message(STATUS "Generating packaging information for TermAPI")
	setup_lib_packaging("TermAPI")
endif()

# Setup packaging for optlib
if (ENABLE_OPTLIB)
	message(STATUS "Generating packaging information for optlib")
	setup_lib_packaging("optlib")
endif()

# Cleanup
unset(PACKAGING_TARGETS CACHE)
unset(PACKAGING_TARGET_NAME CACHE)
