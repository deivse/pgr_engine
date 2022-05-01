file(GLOB test_srcs ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
add_executable(editor ${test_srcs})

set_property(TARGET editor PROPERTY CXX_STANDARD 20)
target_compile_options(editor PUBLIC -g)

target_link_libraries(editor PUBLIC pgre)