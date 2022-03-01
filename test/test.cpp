#include <spdlog/spdlog.h>

#include <app.h>
#include <shader.h>
#include <layers.h>

class test_layer: public npgr::layer_t {

public:
    test_layer(): npgr::layer_t("test_layer_1") {};

    std::set<npgr::event_type> subscribed_event_types() override { 
        return {npgr::event_type::CursorMoved}; 
    }

    void on_attach() override{
        glClearColor(1, 0, 0.5, 1);
    }

    void on_update(const npgr::delta_ms& delta) override {
        // spdlog::info("frame time ms = {}", delta.count());
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void on_event(npgr::event_t& evt) override{
        npgr::event_dispatcher_t ev_d(evt);
        ev_d.dispatch<npgr::cursor_pos_evt_t>([this](npgr::cursor_pos_evt_t& evt){
            spdlog::info("{} handled event: {}({};{})", _debug_name, evt.get_name(), evt.x, evt.y);
            return true;
        });
    }
};

class test_layer_2: public npgr::layer_t {

public:
    test_layer_2(): npgr::layer_t("test_layer_2") {};

    std::set<npgr::event_type> subscribed_event_types() override { 
        return {npgr::event_type::MouseButtonDown, npgr::event_type::CursorMoved}; 
    }

    void on_event(npgr::event_t& evt) override{
        npgr::event_dispatcher_t ev_d(evt);
        ev_d.dispatch<npgr::mouse_btn_down_evt_t>([this](npgr::mouse_btn_down_evt_t& evt){
            spdlog::info("{} handled event: {}({};{})", _debug_name, evt.get_name(), evt.button, evt.mods);
            return true;
        });
        ev_d.dispatch<npgr::cursor_pos_evt_t>([this](npgr::cursor_pos_evt_t& evt){
            spdlog::info("{} handled event: {}({};{})", _debug_name, evt.get_name(), evt.x, evt.y);
            return true;
        });
    }
};

int main() {
    try {
        npgr::app_t app(100, 100, "tesst");
        npgr::shader_program program("test.shader");
        app.push_layer(std::make_unique<test_layer>());
        app.push_layer(std::make_unique<test_layer_2>());
        app.main_loop();
    } catch (std::runtime_error& e) {
        spdlog::error(e.what());
        return 1;
    }
    return 0;
}
