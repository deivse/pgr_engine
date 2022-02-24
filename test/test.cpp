#include <spdlog/spdlog.h>

#include <npgr.h>
#include <shader.h>
#include <layers.h>




int main() {
    try {
        if (auto* window = npgr::init_and_create_window(500, 500, "test", true, 4, 6); window) {
            npgr::shader_program program("../../test.shader");

            layer_stack_t layer_manager;

            auto layerptr = std::make_unique<layer>();

            layer_manager.push_layer(std::move(layerptr));

            npgr::basic_main_loop(window, []() {

            });
        }
    } catch (std::runtime_error& e) {
        spdlog::error(e.what());
        return 1;
    }
    return 0;
}
