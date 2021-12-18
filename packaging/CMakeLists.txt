# packaging
# This file uses the configure_file command to generate packaging files for all subdirectories
cmake_minimum_required(VERSION 3.8)

function(setup_packaging target_name target_targets)
	# Set the cache variable that will be replaced in the copied file
	set(PACKAGING_TARGET_NAME "${target_name}" CACHE STRING "Cache variable used by the packaging handler to determine the current target" FORCE)
	set(PACKAGING_TARGETS "${target_targets}" CACHE STRING "Cache variable used by the packaging handler to determine the current target" FORCE)
	# create the packaging directory if it doesn't exist
	file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/${target_name}/packaging")
	# copy the packaging file to the target
	configure_file("${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt.in" "${CMAKE_SOURCE_DIR}/${target_name}/packaging/CMakeLists.txt" @ONLY)
endfunction()

setup_packaging("shared" "shared")

if (ENABLE_STRLIB)
	setup_packaging("str-lib" "str-lib")
endif()

if (ENABLE_FILELIB)
	setup_packaging("file-lib" "file-lib")
endif()

if (ENABLE_TERMAPI)
	setup_packaging("TermAPI" "ANSI color term extensions TermAPI")
endif()

if (ENABLE_OPTLIB)
	setup_packaging("opt-lib" "opt-lib")
endif()
