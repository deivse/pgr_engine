# Find packages used by this library
find_package(glm REQUIRED CONFIG)
find_package(assimp REQUIRED CONFIG)
find_package(glad REQUIRED CONFIG)
find_package(glfw3 REQUIRED CONFIG)
find_package(imgui REQUIRED CONFIG)
find_package(Jasper REQUIRED CONFIG)
find_package(stb REQUIRED CONFIG)
find_package(libsquish REQUIRED CONFIG)
find_package(spdlog REQUIRED CONFIG)
find_package(EnTT REQUIRED CONFIG)
find_package(cereal REQUIRED CONFIG)

file(GLOB third_party_headers third-party/*/include/*.h)
file(GLOB third_party_cpps third-party/*/src/*.cpp)
file(GLOB headers CONFIGURE_DEPENDS include/*.h include/*/*.h include/*/*/*.h)
file(GLOB cpps CONFIGURE_DEPENDS src/*.cpp src/*/*.cpp src/*/*/*.cpp)

add_library(pgre STATIC ${headers} ${cpps} ${third_party_headers} ${third_party_cpps})

set_property(TARGET pgre PROPERTY CXX_STANDARD 20)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(pgre PUBLIC "PGRE_DISABLE_DEBUG_CHECKS")
endif()

target_compile_definitions(pgre PUBLIC GLFW_INCLUDE_NONE GLM_FORCE_SWIZZLE)
target_include_directories(pgre PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include 
                                       ${CMAKE_CURRENT_SOURCE_DIR}/third-party/imgui/include)

target_link_libraries(pgre 
    glm::glm
    assimp::assimp
    glad::glad
    glfw
    imgui::imgui
    Jasper::Jasper
    stb::stb
    libsquish::libsquish
    spdlog::spdlog
    EnTT::EnTT
    cereal::cereal
)
