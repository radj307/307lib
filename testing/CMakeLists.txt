# 307lib/testing
cmake_minimum_required (VERSION 3.13)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
	# Fix MSVC __cplusplus macro
	string(APPEND CMAKE_CXX_FLAGS " /Zc:__cplusplus")
endif()

project("testing")

file(GLOB HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.h*")
file(GLOB SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_executable(testing ${HEADERS} ${SRC})
target_link_libraries(testing PUBLIC strlib optlib filelib TermAPI)

target_sources(testing PUBLIC ${HEADERS} ${SRC})
target_include_directories(testing PUBLIC 
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
	"$<INSTALL_INTERFACE:include>"
)


