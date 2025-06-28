cmake_minimum_required(VERSION 3.22.2)

project(pgr_engine)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(pgr_engine)

add_subdirectory(pgr_editor)

