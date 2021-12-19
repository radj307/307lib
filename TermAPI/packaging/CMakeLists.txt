# This file was automatically configured by the packaging directory.
# 
# TermAPI/packaging
# src: https://alexreinking.com/blog/building-a-dual-shared-and-static-library-with-cmake.html
cmake_minimum_required(VERSION 3.19)

##### Packaging
include(GNUInstallDirs)				# Adds variables to control the install command. (Platform independant, despite the name)
include(CMakePackageConfigHelpers)	# Adds versioning assistance

# Define the target directory name.
if (NOT DEFINED TermAPI_INSTALL_DIR)
	set(TermAPI_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/TermAPI" CACHE STRING "Path to TermAPI CMake files")
endif()

##### Install

message(STATUS "Installing TermAPI")
message(STATUS "  INSTALL_DIR: \"${TermAPI_INSTALL_DIR}\"")
message(STATUS "  BINARY_DIR:  \"${TermAPI_BINARY_DIR}\"")

# Install export targets
install(
	TARGETS TermAPI
	EXPORT TermAPI_Targets
	RUNTIME COMPONENT TermAPI_Runtime
	LIBRARY COMPONENT TermAPI_Runtime
	NAMELINK_COMPONENT TermAPI_Development
	ARCHIVE COMPONENT TermAPI_Development
	INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

# Install include directories
# To build directory
install(
	DIRECTORY "${TermAPI_SOURCE_DIR}/include/"
	DESTINATION ${TermAPI_BINARY_DIR}
	COMPONENT TermAPI_Development
)
# To installation target
install(
	DIRECTORY "${TermAPI_SOURCE_DIR}/include/"
	TYPE INCLUDE
	COMPONENT TermAPI_Development
)

# Export the target library
install(
	EXPORT TermAPI_Targets
	DESTINATION "${TermAPI_INSTALL_DIR}"
	NAMESPACE 307lib::
	FILE TermAPI-targets.cmake
	COMPONENT TermAPI_Development
)

# Write package versioning config
write_basic_package_version_file(TermAPIConfigVersion.cmake	COMPATIBILITY SameMajorVersion)

# Export config files so find_package can locate the libraries
install(FILES 
	"${CMAKE_CURRENT_SOURCE_DIR}/TermAPI-config.cmake"
	"${CMAKE_CURRENT_BINARY_DIR}/TermAPIConfigVersion.cmake"
	DESTINATION "${TermAPI_INSTALL_DIR}"
	COMPONENT TermAPI_Development
)

# Set the TermAPI_ROOT variable
set(TermAPI_ROOT "${TermAPI_INSTALL_DIR}" CACHE STRING "Set the search directory for find_package" FORCE) # Set the find_package search directory
