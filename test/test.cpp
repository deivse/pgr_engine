#include <spdlog/spdlog.h>

#include <app.h>
#include <glm/glm.hpp>
#include <shader_program.h>
#include <layers.h>
#include <layers/imgui_layer.h>
#include <layers/basic_layer.h>
#include <gl_wrappers/vertex_array.h>

namespace glwr = pgre::gl_wrappers;

class test_layer: public pgre::layers::basic_layer_t {
    std::unique_ptr<glwr::vertex_array_t> vao;
    pgre::shader_program_t program;
    std::shared_ptr<glwr::buffer_t> buffer;
    glm::vec3 color{1.0, 0.0, 0.0};

    float data[9] = {
      0.5F,  0.5F,  0.0F, // top right
      0.5F,  -0.5F, 0.0F, // bottom right
      -0.5F, 0.5F,  0.0F,
    };
    float velocity = 1.F; 

public:
    test_layer(): pgre::layers::basic_layer_t("test_layer_1") {};

    void on_attach() override{
        buffer = std::make_shared<glwr::buffer_t>(GL_ARRAY_BUFFER);
        program = pgre::shader_program_t("./test.shader");
        vao = std::make_unique<glwr::vertex_array_t>();

        vao->add_attribute(*buffer, program.get_attrib_location("position"), GL_FLOAT, 3, false, 0, 0);
        vao->set_vertex_count(3);
        buffer->unbind();        
    }

    void on_update(const pgre::layers::delta_ms& delta) override {
        glClear(GL_COLOR_BUFFER_BIT);
        program.bind();
        color.b += velocity * (delta.count() / 1000.F);
        color.r -= 2 * velocity * (delta.count() / 1000.F);

        program.set_uniform(glUniform3f, "color", color.r, color.g, color.b);

        for (int i = 0; i < 3; i++) {
            data[i] += velocity * (delta.count() / 1000.F);
            if (data[i]>1.F || data[i] < -1.F) velocity = -velocity;
        }
        buffer->set_data(sizeof(data), data, GL_DYNAMIC_DRAW);
        vao->draw_arrays();
    }

    bool on_event(pgre::event_t& evt) override {
        pgre::event_dispatcher_t ev_d(evt);
        ev_d.dispatch<pgre::cursor_pos_evt_t>([this](pgre::cursor_pos_evt_t& evt){
            // spdlog::info("{} handled event: {}({};{})", _debug_name, evt.get_name(), evt.x, evt.y);
            return true;
        });
        return false;
    }
};

class test_layer_2: public pgre::layers::basic_layer_t {

public:
    test_layer_2(): pgre::layers::basic_layer_t("test_layer_2") {};


    bool on_event(pgre::event_t& evt) override{
        pgre::event_dispatcher_t ev_d(evt);
        ev_d.dispatch<pgre::mouse_btn_down_evt_t>([this](pgre::mouse_btn_down_evt_t& evt){
            // spdlog::info("{} handled event: {}({};{})", _debug_name, evt.get_name(), evt.button, evt.mods);
            return true;
        });
        return false;
    }
};

class imgui_overlay : public pgre::layers::imgui_layer_t {
};

int main() {
    // spdlog::set_level(spdlog::level::warn);
    try {
        pgre::app_t app(480, 240, "tesst");
        
        app.push_overlay(std::make_unique<imgui_overlay>());
        app.push_layer(std::make_unique<test_layer>());
        app.push_layer(std::make_unique<test_layer_2>());
        app.main_loop();
    } catch (std::runtime_error& e) {
        spdlog::error(e.what());
        return 1;
    }
    return 0;
}
