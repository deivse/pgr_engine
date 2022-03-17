#include <spdlog/spdlog.h>

#include <app.h>
#include <shader_program.h>
#include <layers.h>
#include <layers/imgui_layer.h>
#include <layers/basic_layer.h>

class test_layer: public npgr::layers::basic_layer_t {

public:
    test_layer(): npgr::layers::basic_layer_t("test_layer_1") {};

    void on_attach() override{
        glClearColor(1, 0, 0.5, 1);
    }

    void on_update(const npgr::layers::delta_ms& delta) override {
        // spdlog::info("frame time ms = {}", delta.count());
        glClear(GL_COLOR_BUFFER_BIT);
    }

    bool on_event(npgr::event_t& evt) override {
        npgr::event_dispatcher_t ev_d(evt);
        ev_d.dispatch<npgr::cursor_pos_evt_t>([this](npgr::cursor_pos_evt_t& evt){
            // spdlog::info("{} handled event: {}({};{})", _debug_name, evt.get_name(), evt.x, evt.y);
            return true;
        });
        return false;
    }
};

class test_layer_2: public npgr::layers::basic_layer_t {

public:
    test_layer_2(): npgr::layers::basic_layer_t("test_layer_2") {};


    bool on_event(npgr::event_t& evt) override{
        npgr::event_dispatcher_t ev_d(evt);
        ev_d.dispatch<npgr::mouse_btn_down_evt_t>([this](npgr::mouse_btn_down_evt_t& evt){
            // spdlog::info("{} handled event: {}({};{})", _debug_name, evt.get_name(), evt.button, evt.mods);
            return true;
        });
        return false;
    }
};

class imgui_overlay : public npgr::layers::imgui_layer_t {
};

int main() {
    spdlog::set_level(spdlog::level::warn);
    try {
        npgr::app_t app(480, 240, "tesst");
        npgr::shader_program_t program("./test.shader");
        program.set_uniform(glUniform1f, "test", 1.F);
        
        app.push_layer(std::make_unique<test_layer>());
        app.push_layer(std::make_unique<test_layer_2>());
        app.push_overlay(std::make_unique<imgui_overlay>());
        app.main_loop();
    } catch (std::runtime_error& e) {
        spdlog::error(e.what());
        return 1;
    }
    return 0;
}
