# 307lib/testing/UnitTests
cmake_minimum_required(VERSION 3.20)

# Disable GTest installation targets
set(INSTALL_GTEST OFF CACHE BOOL "Enable installation of googletest. (Projects embedding googletest may want to turn this OFF.)" FORCE)
set(BUILD_GMOCK ON CACHE BOOL "Builds the googlemock subproject" FORCE)

# Add the googletest subdirectory
add_subdirectory("googletest")

# Add global include dirs for all sources

# Include ctest & googletest functions. This calls enable_testing()
include(CTest)
include(GoogleTest)

# add global dependencies for all tests
link_libraries(gmock)
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/googletest/googletest/include;${CMAKE_CURRENT_SOURCE_DIR}/googletest/googletest/src;${CMAKE_CURRENT_SOURCE_DIR}/googletest/googlemock/include;${CMAKE_CURRENT_SOURCE_DIR}/googletest/googlemock/src")




# Add test subdirectories
add_subdirectory("TEST_ArgumentRedux")

