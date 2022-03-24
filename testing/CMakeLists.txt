# 307lib/testing
cmake_minimum_required (VERSION 3.20)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

option(307lib_ENABLE_DEVELOPMENT "Enable 307lib's development testing subproject." OFF)
if(307lib_ENABLE_DEVELOPMENT)
	# Collect files
	file(GLOB HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.h*")
	file(GLOB SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

	# Add executable
	add_executable(testing ${HEADERS} ${SRC})

	# Set properties
	set_property(TARGET testing PROPERTY CXX_STANDARD 20)
	set_property(TARGET testing PROPERTY CXX_STANDARD_REQUIRED ON)
	if (MSVC)
		target_compile_options(testing PUBLIC "/Zc:__cplusplus" "/Zc:preprocessor")
	endif()

	target_link_libraries(testing PUBLIC shared TermAPI strlib optlib filelib)

	target_sources(testing PUBLIC ${HEADERS} ${SRC})
	target_include_directories(testing PUBLIC 
		"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
		"$<INSTALL_INTERFACE:include>"
	)
endif()

# Enables Unit Tests using googletest & googlemock.
option(307lib_ENABLE_UNIT_TESTING "Enable the unit testing subproject." OFF)
if (307lib_ENABLE_UNIT_TESTING AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/UnitTests")
	message(STATUS "Unit Testing is enabled.")
	add_subdirectory("UnitTests")
endif()