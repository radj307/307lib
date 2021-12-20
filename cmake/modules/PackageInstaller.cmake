# 307lib/cmake/modules/PackageInstaller.cmake
# Contains functions for creating packaging files & installing libraries.
cmake_minimum_required(VERSION 3.19)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(GenerateExportHeader)

#### GENERATE_PACKAGING ####
# @brief				Generates an export header for the target library, and creates a Config.cmake file. Must be called from the 
# @param _name			The name of the target library. This must be a direct subdirectory of CMAKE_SOURCE_DIR, and the name of the library target.
function(generate_packaging _name)
	# Export Targets
	file(REMOVE "${CMAKE_CURRENT_BINARY_DIR}/export.h") # remove any existing export header
	generate_export_header(${_name} EXPORT_FILE_NAME "${CMAKE_CURRENT_BINARY_DIR}/export.h")
	# Create <name>Config.cmake from the template
	set(PACKAGING_TARGET_NAME "${_name}" CACHE STRING "Temporary variable used by the GENERATE_PACKAGING function." FORCE)
	set(PACKAGING_TARGET_CONF "${CMAKE_CURRENT_BINARY_DIR}/${_name}Config.cmake")
	file(REMOVE ${PACKAGING_TARGET_CONF}) # remove any existing package config
	configure_file("${CMAKE_SOURCE_DIR}/cmake/input/config.cmake.in" "${PACKAGING_TARGET_CONF}" @ONLY)
	message(STATUS "Successfully generated ${PACKAGING_TARGET_CONF}")
	unset(PACKAGING_TARGET_NAME CACHE)
endfunction()

#### WRITE_VERSION_FILE ####
# @brief				Creates a ConfigVersion file for the given library name.
# @param _name			The name of the target library. This must be a direct subdirectory of CMAKE_SOURCE_DIR, and the name of the library target.
# @param _compat_mode	The compatibility mode to use in the version file.
function(write_version_file _name _compat_mode)
	write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/${_name}ConfigVersion.cmake" COMPATIBILITY ${_compat_mode})
endfunction()

#### CREATE_PACKAGE ####
# @brief				Calls the GENERATE_PACKAGING function, then generates the ConfigVersion file using the provided compatiblity mode.
# @param _name			The name of the target library. This must be a direct subdirectory of CMAKE_SOURCE_DIR, and the name of the library target.
# @param _compat_mode	The compatibility mode to use in the version file.
function(create_package _name _compat_mode)
	GENERATE_PACKAGING(${_name})
	write_basic_package_version_file("${CMAKE_CURRENT_SOURCE_DIR}/${_name}ConfigVersion.cmake" COMPATIBILITY ${_compat_mode})
endfunction()

#### INSTALL_PACKAGE ####
# @brief				Installs the specified package.
# @param _name			The name of the target library. This must be a direct subdirectory of CMAKE_SOURCE_DIR, and the name of the library target.
function(install_package _name)
	# Define <name>_INSTALL_DIR
	if (NOT DEFINED "${_name}_CONFIG_INSTALL_DIR")
		set("${_name}_CONFIG_INSTALL_DIR" "${CMAKE_INSTALL_LIBDIR}/cmake/${_name}" CACHE STRING "Path to ${_name} packaging configs.")
	endif()

	message(STATUS "Creating packaging requirements for ${_name}")
		
	install( # Install targets to CMAKE_INSTALL_INCLUDEDIR
		TARGETS ${_name}
		EXPORT "${_name}_Targets"
		RUNTIME COMPONENT "${_name}_Runtime"
		LIBRARY COMPONENT "${_name}_Runtime"
		NAMELINK_COMPONENT "${_name}_Development"
		ARCHIVE COMPONENT "${_name}_Development"
		INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
	)
	
	install( # Install package config files
		FILES
			"$<IF:$<BOOL:EXISTS ${CMAKE_CURRENT_BINARY_DIR}/${_name}ConfigVersion.cmake>,${CMAKE_CURRENT_BINARY_DIR}/${_name}Config.cmake;${CMAKE_CURRENT_BINARY_DIR}/${_name}ConfigVersion.cmake,${CMAKE_CURRENT_BINARY_DIR}/${_name}Config.cmake>"
			${CFG_FILES}
		DESTINATION "${${_name}_INSTALL_DIR}"
		COMPONENT "${_name}_Development"
	)
	
	install( # Install associated include directory to development component
		DIRECTORY "include/"
		TYPE INCLUDE
		COMPONENT "${_name}_Development"
	)
	
	install( # export the library configs
		EXPORT "${_name}_Targets"
		DESTINATION "${${_name}_CONFIG_INSTALL_DIR}"
		NAMESPACE 307lib::
		FILE "${_name}-targets.cmake"
		COMPONENT "${_name}_Development"
	)

	# Set the <name>_ROOT environment variable
	set(ENV{${_name}_ROOT} "${${_name}_CONFIG_INSTALL_DIR}")
endfunction()
