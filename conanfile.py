from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps, cmake_layout


class PgrEngineConan(ConanFile):
    name = "pgr_engine"
    version = "1.0"
    
    description = "PGR Engine - Graphics Engine Project"
    author = "Your Name"
    
    settings = "os", "compiler", "build_type", "arch"
    exports_sources = "CMakeLists.txt", "pgr_engine/*", "pgr_editor/*"
    
    requires = (
        "glm/0.9.9.8",
        "glfw/3.3.6",
        "glad/0.1.36",
        "assimp/5.4.3",
        "stb/cci.20230920",
        "imgui/cci.20220207+1.87.docking",
        "jasper/2.0.33",
        "libsquish/1.15",
        "spdlog/1.12.0",
        "entt/3.9.0",
        "cereal/1.3.1",
    )
    build_requires = (
        "cmake/[>=3.22]",
        "ninja/[>=1.10.2]",
    )
    default_options = {
        "spdlog/*:shared": False,
        "glad/*:gl_version": "4.5",
        "glad/*:gl_profile": "core",
        "stb/*:with_deprecated": False,
    }
    
    def layout(self):
        cmake_layout(self)
    
    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        
        tc = CMakeToolchain(self, generator="Ninja")
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure() 
        cmake.build()
