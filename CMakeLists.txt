cmake_minimum_required(VERSION 3.22.2)

project(pgr_engine)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD 20)
if (NOT CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS -g)
endif()

add_subdirectory(pgr_engine)

add_subdirectory(pgr_editor)

