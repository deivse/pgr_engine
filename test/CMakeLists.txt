file(GLOB test_srcs ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
add_executable(test ${test_srcs})

set_property(TARGET test PROPERTY CXX_STANDARD 20)

target_link_libraries(test PUBLIC npgr)