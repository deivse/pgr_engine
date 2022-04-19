#include <spdlog/spdlog.h>

#include <app.h>
#include <glm/glm.hpp>
#include <assets/shader_program.h>
#include <layers.h>
#include <layers/imgui_layer.h>
#include <layers/basic_layer.h>
#include <primitives/vertex_array.h>
#include <events/keyboard_events.h>
#include <input/keyboard.h>
#include <assets/texture.h>
#include <renderer/camera.h>
#include <scene/scene.h>
#include <scene/entity.h>

using pgre::assets::texture2D_t;

namespace glwr = pgre::primitives;

std::stringstream shader_source(R"(
shader::vertex {
    #version 400
    uniform mat4 PVMmatrix;
    in vec3 position;
    in vec4 color;

    smooth out vec4 theColor;
    smooth out vec4 ver_position;
    smooth out vec2 v_tex_coord;
    
    void main() {
	  gl_Position = PVMmatrix * vec4(position, 1.0);
	  theColor = color;
      ver_position = gl_Position;
      v_tex_coord = position.xy / 2 + 0.5 ;
    }
} shader::vertex

shader::fragment {
    #version 400
    smooth in vec4 theColor;
    smooth in vec4 ver_position;
    smooth in vec2 v_tex_coord;

    uniform sampler2D u_texture;
    out vec4 outputColor;

    void main(){
        outputColor = texture(u_texture, v_tex_coord);
    }
} shader::fragment
)");

class test_layer: public pgre::layers::basic_layer_t {
    std::unique_ptr<glwr::vertex_array_t> vao;
    pgre::shader_program_t program;
    std::shared_ptr<glwr::vertex_buffer_t> buffer;
    glm::vec3 color{1.0, 0.0, 0.0};
    pgre::camera_t camera;

    std::shared_ptr<texture2D_t> test_texture = std::make_shared<texture2D_t>(
      "/home/deivse/Documents/sem6/PGR/desiaiva_framework/test/assets/test_texture.png",
      GL_NEAREST);
    std::shared_ptr<texture2D_t> cross_texture = std::make_shared<texture2D_t>(
      "/home/deivse/Documents/sem6/PGR/desiaiva_framework/test/assets/cross.png",
      GL_NEAREST);

    static constexpr float camera_speed = 2.;

    float data[24] = {
      0.5F,  0.5F,  -1.0F, 0.8F, 0.1F, 0.2F, 1.0F, // top right
      0.5F,  -0.5F, -1.0F, 0.8F, 0.1F, 0.7F, 1.0F, // bottom right
      -0.5F, 0.5F,  -1.0F, 0.2F, 0.5F, 0.7F, 1.0F,
    };

    void process_input(float delta_secs){
        using namespace pgre::input;
        glm::vec3 camera_translation {0.0F};
        if (key_down(GLFW_KEY_W)) camera_translation.z = -1;
        else if (key_down(GLFW_KEY_S)) camera_translation.z = 1;
        
        else {
            return;
        }
        
        camera_translation = glm::normalize(camera_translation) * delta_secs * camera_speed;
        auto& cam_pos = camera.get_position();
        cam_pos += camera_translation;
        auto& cam_orien = camera.get_orientation();

        camera.update_view_matrix();
    }

public:
    test_layer(): pgre::layers::basic_layer_t("test_layer_1") {};

    void on_attach() override{
        program = pgre::shader_program_t(shader_source);
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

        program.bind();
        program.set_uniform("u_texture", 0);
    }

    void on_update(const pgre::interval_t& delta) override {
        glClear(GL_COLOR_BUFFER_BIT);

        process_input(delta.seconds);
        program.bind();
        //TODO: perspective
        auto win_dims = pgre::app_t::get_window().get_dimensions();
        
        auto projectionMatrix = glm::perspective(glm::radians(60.0F), win_dims.width/win_dims.height, 0.1F, 10.0F);
        glm::mat4 PV = projectionMatrix * camera.get_view_matrix();

        program.set_uniform("PVMmatrix", PV);

        vao->bind();
        test_texture->bind(0);
        glDrawArrays(GL_TRIANGLES, 0, buffer->get_size()/sizeof(float));
        cross_texture->bind(0);
        program.set_uniform("PVMmatrix", glm::translate(PV, {-0.25, -0.25, 0.5}));
        glDrawArrays(GL_TRIANGLES, 0, buffer->get_size()/sizeof(float));
    }

    bool on_event(pgre::event_t& evt) override {
        // pgre::event_dispatcher_t ev_d(evt);
        // ev_d.dispatch<pgre::key_pressed_evt_t>([this](pgre::key_pressed_evt_t& evt){
        //     if (evt.key == GLFW_KEY_W) {
        //         camera.set_position(camera.get_position()+0.02F);
        //         camera.update_view_matrix();
        //     }
        //     return true;
        // });
        return false;
    }
};

struct test_component_t {
    float vel;
};

class test_layer_2: public pgre::layers::basic_layer_t {
    pgre::scene::scene_t scene;
public:
    test_layer_2(): pgre::layers::basic_layer_t("test_layer_2") {};

    void on_attach() override {
        auto entity = scene.create_entity();
        auto x = entity.add_component<test_component_t>();
        x.vel = 0.4;
    }

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
        spdlog::set_level(spdlog::level::level_enum::trace);
        pgre::app_t app(1280, 720, "tesst", false);
        
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
