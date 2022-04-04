cmake_minimum_required(VERSION 3.22.2)

project(pgr_engine)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

set(CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(pgr_engine)

add_subdirectory(test)

