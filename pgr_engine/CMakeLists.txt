file(GLOB third_party_headers third-party/*/include/*.h)
file(GLOB third_party_cpps third-party/*/src/*.cpp)
file(GLOB headers include/*.h include/*/*.h)
file(GLOB cpps src/*.cpp src/*/*.cpp)

add_library(pgre STATIC ${headers} ${cpps} ${third_party_headers} ${third_party_cpps})

set_property(TARGET pgre PROPERTY CXX_STANDARD 20)

target_compile_definitions(pgre PUBLIC GLFW_INCLUDE_NONE)
target_include_directories(pgre PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include 
                                       ${CMAKE_CURRENT_SOURCE_DIR}/third-party/imgui/include)

target_link_libraries(pgre ${CONAN_LIBS})

target_compile_options(pgre PUBLIC -g)