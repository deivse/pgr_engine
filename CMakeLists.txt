cmake_minimum_required(VERSION 3.22.2)

project(not_pgr_framework)


include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_subdirectory(third-party)

set(CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(npgr_framework)

add_subdirectory(test)

