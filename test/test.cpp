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
    std::shared_ptr<glwr::vertex_buffer_t> buffer;
    glm::vec3 color{1.0, 0.0, 0.0};

    float data[24] = {
      0.5F,  0.5F,  0.0F, 0.8F, 0.1F, 0.2F, 1.0F, // top right
      0.5F,  -0.5F, 0.0F, 0.8F, 0.1F, 0.7F, 1.0F, // bottom right
      -0.5F, 0.5F,  0.0F, 0.2F, 0.5F, 0.7F, 1.0F,
    };
    float velocity = 1.F; 

public:
    test_layer(): pgre::layers::basic_layer_t("test_layer_1") {};

    void on_attach() override{
        program = pgre::shader_program_t("./test.shader");
        buffer = std::make_shared<glwr::vertex_buffer_t>();
        buffer->set_data(sizeof(data), data, GL_DYNAMIC_DRAW);
        vao = std::make_unique<glwr::vertex_array_t>();
        auto layout = std::make_shared<glwr::buffer_layout_t>(
            program, std::initializer_list<glwr::buffer_element_t>{{GL_FLOAT, 3, "position"}}
        );
        vao->add_vertex_buffer(buffer, {program, {
            {GL_FLOAT, 3, "position"},
            {GL_FLOAT, 4, "color"},
        }});
        buffer->unbind();
    }

    void on_update(const pgre::layers::delta_ms& delta) override {
        glClear(GL_COLOR_BUFFER_BIT);
        program.bind();
        vao->bind();
        glDrawArrays(GL_TRIANGLES, 0, buffer->get_size()/sizeof(float));
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
