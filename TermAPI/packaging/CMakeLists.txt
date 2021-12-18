# TermAPI/packaging
# src: https://alexreinking.com/blog/building-a-dual-shared-and-static-library-with-cmake.html
cmake_minimum_required(VERSION 3.12)

##### Packaging
include(GNUInstallDirs)				# Adds variables to control the install command. (Platform independant, despite the name)
include(CMakePackageConfigHelpers)	# Adds versioning assistance

# Define install directory
if (NOT DEFINED TermAPI_INSTALL_DIR)
	set(TermAPI_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/TermAPI" CACHE STRING "Path to TermAPI CMake files")
endif()

##### Install

# Install target library headers
install(
	TARGETS ANSI color term extensions TermAPI
	EXPORT TermAPI_Targets
	RUNTIME COMPONENT TermAPI_Runtime
	LIBRARY COMPONENT TermAPI_Runtime
	NAMELINK_COMPONENT TermAPI_Development
	ARCHIVE COMPONENT TermAPI_Development
	INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)
install(
	DIRECTORY "${TermAPI_SOURCE_DIR}/include/" "${TermAPI_BINARY_DIR}/include/"
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

# Export the TargetLibConfig.cmake file so find_package can locate the libraries
install(
	FILES "${CMAKE_CURRENT_SOURCE_DIR}/TermAPI-config.cmake"
	DESTINATION "${TermAPI_INSTALL_DIR}"
	COMPONENT TermAPI_Development
)
