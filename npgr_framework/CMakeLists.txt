file(GLOB third_party_headers third-party/*/include/*.h)
file(GLOB third_party_cpps third-party/*/src/*.cpp)
file(GLOB npgr_headers include/*.h include/*/*.h)
file(GLOB npgr_cpps src/*.cpp src/*/*.cpp)

add_library(npgr STATIC ${npgr_headers} ${npgr_cpps} ${third_party_headers} ${third_party_cpps})

set_property(TARGET npgr PROPERTY CXX_STANDARD 20)

target_compile_definitions(npgr PUBLIC GLFW_INCLUDE_NONE)
target_include_directories(npgr PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include 
                                       ${CMAKE_CURRENT_SOURCE_DIR}/third-party/imgui/include)
target_link_libraries(npgr ${CONAN_LIBS} IL)

target_compile_options(npgr PUBLIC -g)