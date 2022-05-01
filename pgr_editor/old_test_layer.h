#pragma once
#include <spdlog/spdlog.h>

#include <app.h>
#include <glm/glm.hpp>
#include <primitives/shader_program.h>
#include <layers.h>
#include <layers/imgui_layer.h>
#include <layers/basic_layer.h>
#include <primitives/vertex_array.h>
#include <events/keyboard_events.h>
#include <input/keyboard.h>
#include <assets/texture.h>
#include <assets/phong_material.h>
#include <renderer/camera.h>
#include <scene/scene.h>
#include <scene/entity.h>


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

std::stringstream flat_shader_source{R"(
shader::vertex {
    #version 400
    uniform mat4 PVMmatrix;
    in vec3 position;
    
    void main() {
	  gl_Position = PVMmatrix * vec4(position, 1.0);
    }
} shader::vertex

shader::fragment {
    #version 400
    uniform vec4 u_color;
    out vec4 outputColor;

    void main(){
        outputColor = u_color;
    }
} shader::fragment
)"};

class test_layer: public pgre::layers::basic_layer_t {
    std::unique_ptr<glwr::vertex_array_t> vao;
    pgre::shader_program_t program;
    std::shared_ptr<glwr::vertex_buffer_t> buffer;
    glm::vec3 color{1.0, 0.0, 0.0};
    pgre::perspective_camera_t camera = pgre::perspective_camera_t(90);

    std::shared_ptr<pgre::texture2D_t> test_texture = std::make_shared<pgre::texture2D_t>(
      "/home/deivse/Documents/sem6/PGR/desiaiva_framework/test/assets/test_texture.png",
      GL_NEAREST);
    std::shared_ptr<pgre::texture2D_t> cross_texture = std::make_shared<pgre::texture2D_t>(
      "/home/deivse/Documents/sem6/PGR/desiaiva_framework/test/assets/cross.png",
      GL_NEAREST);

    static constexpr float camera_speed = 2.;

    float data[24] = {
      10.5F,  10.5F,  10.0F, 0.8F, 0.1F, 0.2F, 1.0F, // top right
      10.5F,  -10.5F, 10.0F, 0.8F, 0.1F, 0.7F, 1.0F, // bottom right
      -10.5F, 10.5F,  10.0F, 0.2F, 0.5F, 0.7F, 1.0F,
    };

public:
    test_layer(): pgre::layers::basic_layer_t("test_layer_1") {};

    void on_attach() override{
        program = pgre::shader_program_t(shader_source);
        // pgre::phong_material_t({1, 1, 1}, {0.2, 1, 1}, {1,1,1});
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

        program.bind();
        //TODO: perspective
        auto win_dims = pgre::app_t::get_window().get_dimensions();
        

        glm::mat4 PV = camera.get_PV_matrix();

        program.set_uniform("PVMmatrix", PV);

        vao->bind();
        test_texture->bind(0);
        program.set_uniform("u_texture", 0);

        glDrawArrays(GL_TRIANGLES, 0, buffer->get_size()/sizeof(float));
        cross_texture->bind(0);
        program.set_uniform("u_texture", 0);

        // program.set_uniform("u_color", glm::vec4(0.0, 0.0, 1.0, 0.));
        // program.set_uniform("PVMmatrix", glm::translate(PV, {-0.25, -0.25, 0.5}));
        
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
