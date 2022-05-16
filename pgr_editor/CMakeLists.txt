file(GLOB test_srcs CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/*/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h ${CMAKE_CURRENT_SOURCE_DIR}/src/*/*.h)
add_executable(editor ${test_srcs})

set_property(TARGET editor PROPERTY CXX_STANDARD 20)
target_compile_options(editor PUBLIC -g)

target_link_libraries(editor PUBLIC pgre)