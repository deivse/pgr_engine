cmake_minimum_required(VERSION 3.22.2)

project(not_pgr_framework)


include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_subdirectory(third-party)

