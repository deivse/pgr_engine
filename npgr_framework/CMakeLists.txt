file(GLOB npgr_headers include/*)
file(GLOB npgr_cpps src/*)



add_library(npgr STATIC ${npgr_headers} ${npgr_cpps})

set_property(TARGET npgr PROPERTY CXX_STANDARD 20)

target_include_directories(npgr PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(npgr ${CONAN_LIBS} IL)