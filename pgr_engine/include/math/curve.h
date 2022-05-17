#pragma once
#include "assets/materials/flat_color_material.h"
#include "glm/mat4x4.hpp"
#include "primitives/vertex_array.h"
#include <glm/vec3.hpp>
#include <vector>

#include <cerealization/glm_serializers.h>
#include <cereal/types/vector.hpp>

namespace pgre::math::curves {

struct coons_curve_3D_t {
    constexpr static glm::mat4 basis_coefficients = {-1, 3, -3, 1, 3, -6, 0, 4, -3, 3, 3, 1, 1, 0, 0, 0};
    std::vector<glm::vec3> control_points;
    std::shared_ptr<primitives::vertex_array_t> vao
      = std::make_shared<primitives::vertex_array_t>();
    std::shared_ptr<primitives::vertex_buffer_t> vbo
      = std::make_shared<primitives::vertex_buffer_t>();
    std::shared_ptr<primitives::index_buffer_t> ebo
      = std::make_shared<primitives::index_buffer_t>();
    inline static const auto point_material
          = std::make_shared<flat_color_material_t>(glm::vec3{0.8, 0.2, 0.5});

    void fill_buffers(){
        if (!control_points.empty()){
            vbo->set_data(control_points.size()*3*sizeof(float), glm::value_ptr(control_points[0]));
            std::vector<GLuint> ix_data; 
            for (size_t i = 0; i < control_points.size(); i++){
                ix_data.push_back(i);
            }
            ebo->set_data(ix_data);
            vao->add_vertex_buffer(
              vbo, std::make_shared<primitives::buffer_layout_t>(
                     std::initializer_list<primitives::buffer_element_t>{{GL_FLOAT, 3, "position"}},
                     point_material));
            vao->set_index_buffer(ebo);
        }
    };
public:

    /**
     * @brief Gets a VAO filled with the curves control points. 
     * 
     * @return std::shared_ptr<primitives::vertex_array_t> 
     */
    std::shared_ptr<primitives::vertex_array_t> get_cp_vao(){
        fill_buffers();
        return vao;
    }

    std::shared_ptr<material_t> get_material(){
        return point_material;
    }

    auto get_pos_on_curve(float t, size_t first_cp){
        first_cp = first_cp % control_points.size();

        const glm::mat3x4 segment_control_points = glm::transpose(glm::mat4x3{
          control_points[first_cp], control_points[(first_cp + 1) % control_points.size()],
          control_points[(first_cp + 2) % control_points.size()],
          control_points[(first_cp + 3) % control_points.size()]});

        return 0.166667f * glm::vec4(std::pow(t, 3), std::pow(t, 2), t, 1) * basis_coefficients
               * segment_control_points;
    }

    auto get_direction_of_curve(float t, size_t first_cp){
        first_cp = first_cp % control_points.size();

        const glm::mat3x4 segment_control_points = glm::transpose(glm::mat4x3{
          control_points[first_cp], control_points[(first_cp + 1) % control_points.size()],
          control_points[(first_cp + 2) % control_points.size()],
          control_points[(first_cp + 3) % control_points.size()]});

        return 0.166667f * glm::vec4(3 * std::pow(t, 2), 2 * std::pow(t, 1), 1, 0)
               * basis_coefficients * segment_control_points;
    }

    void remove_point(size_t ix) {
        control_points.erase(std::begin(control_points) + ix);
        
    }

    auto& get_point(size_t ix) {
        return control_points[ix];
    }

    auto& get_point_vec() {
        return control_points;
    
    }

    [[nodiscard]] size_t get_point_count() const{
        return control_points.size();
    }

    void add_point(size_t insert_after_ix, const glm::vec3& cp_pos) {
        control_points.insert(std::begin(control_points) + insert_after_ix, cp_pos);
        
    }

    template <typename ArchiveTy>
    void serialize(ArchiveTy& ar) {
        ar(control_points);
    }
};


}  // namespace pgre::math::curves