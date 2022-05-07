#include <spdlog/spdlog.h>

#include <app.h>


#include "scene_gui.h"
#include "scene_layer.h"


int main() {
    // spdlog::set_level(spdlog::level::warn);
    try {
        pgre::app_t app(1920, 1080, ":)))))))", false);
        spdlog::set_level(spdlog::level::level_enum::info);
        std::shared_ptr<pgre::scene::scene_t> scene{};
        auto scene_l = std::make_shared<scene_layer_t>(scene);
        app.push_layer(scene_l);
        app.push_overlay(std::make_unique<scene_gui_layer_t>(scene_l));
        app.main_loop();
    } catch (std::runtime_error& e) {
        spdlog::error(e.what());
        return 1;
    }
    return 0;
}
