# 307lib/filelib

# Retrieve headers & sources
file(GLOB HEADERS
	RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
	CONFIGURE_DEPENDS
	"include/*.h*"
)

# Create header-only library
add_library(filelib INTERFACE)
add_library(307lib::filelib ALIAS filelib)

# Set properties
set_property(TARGET filelib PROPERTY CXX_STANDARD 20)
set_property(TARGET filelib PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET filelib PROPERTY POSITION_INDEPENDENT_CODE ON)
set_property(TARGET filelib PROPERTY WINDOWS_EXPORT_ALL_SYMBOLS ON)

target_compile_options(filelib INTERFACE "${307lib_compiler_commandline}")

target_include_directories(filelib INTERFACE 
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
	"$<INSTALL_INTERFACE:include;src>"
)

# Link library dependencies
target_link_libraries(filelib INTERFACE shared)

# Create library installation targets
if (307lib_ENABLE_PACKAGING) # Include packaging
	include(PackageInstaller)
	GENERATE_PACKAGING(filelib)
	WRITE_VERSION_FILE(filelib SameMajorVersion)
	INSTALL_PACKAGE(filelib)
endif()
