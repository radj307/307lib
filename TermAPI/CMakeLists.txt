# TermAPI/v4
cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
	# Fix MSVC __cplusplus macro
	string(APPEND CMAKE_CXX_FLAGS " /Zc:__cplusplus")
endif()


option(TERMAPI_VERSION_3 "Use TermAPI version 3 instead of v4." OFF)
if (TERMAPI_VERSION_3)
	message(STATUS "Using TermAPI v3")
	add_subdirectory("v3")
else()
	message(STATUS "Using TermAPI v4")
	
	project("TermAPI")

	# Add sequence library
	add_subdirectory("ANSI")
		
	set(HEADERS
		"./include/TermAPI.hpp"
	)
	set(SRC
		"./src/TermAPI.cpp"
	)

	add_library(TermAPI STATIC ${HEADERS} ${SRC})
	target_include_directories(TermAPI PUBLIC "./include" "./src")
	target_link_directories(TermAPI PUBLIC "./src")

	add_subdirectory("color")
	add_subdirectory("term")

	target_link_libraries(TermAPI PUBLIC ANSI color term)

	include(CMakeDependentOption)
	cmake_dependent_option(TERMAPI_ENABLE_EXTENSIONS "Enable the TermAPI extensions library" ON "ENABLE_FILELIB" OFF)
	if(TERMAPI_ENABLE_EXTENSIONS)
		add_subdirectory("extensions")
		target_link_libraries(TermAPI PUBLIC file-lib extensions)
	endif()
endif()