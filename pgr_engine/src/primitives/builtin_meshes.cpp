#include <primitives/builtin_meshes.h>


namespace pgre::builtin_meshes {

namespace {
    constexpr float cube_vertices[24]
      = {-1, -1, -1, // bottom close left  0
          1, -1, -1, // bottom close right 1
          1,  1, -1, // bottom far right   2
         -1,  1, -1, // bottom far left    3
         -1, -1,  1, // top close left     4
          1, -1,  1, // top close right    5
          1,  1,  1, // top far right      6
         -1,  1,  1};// top far left       7

    constexpr unsigned int cube_indices[36] = {0, 1, 3, 3, 1, 2, 1, 5, 2, 2, 5, 6, 5, 4, 6, 6, 4, 7,
                                         4, 0, 7, 7, 0, 3, 3, 2, 7, 7, 2, 6, 4, 5, 0, 0, 5, 1};
} // namespace

std::shared_ptr<primitives::vertex_array_t>
  get_cube_vao(const std::shared_ptr<material_t>& material, bool normals, bool tex_coords) {
    constexpr auto make_vao = [](auto& material){
        auto vao = std::make_shared<primitives::vertex_array_t>();
        auto ebo = std::make_shared<primitives::index_buffer_t>();
        auto vbo = std::make_shared<primitives::vertex_buffer_t>();

        vbo->set_data(sizeof(float) * 24, reinterpret_cast<const void*>(cube_vertices));
        ebo->set_data(sizeof(unsigned int) * 36, reinterpret_cast<const void*>(cube_indices));
        vao->add_vertex_buffer(
          vbo, std::make_shared<primitives::buffer_layout_t>(
                 std::initializer_list<primitives::buffer_element_t>{{GL_FLOAT, 3, "position"}}, material));
        vao->set_index_buffer(ebo);
        return vao;
    };
    //TODO: shit code, optimize, don't create every time.
    if (normals || tex_coords)
        throw std::runtime_error("Builtin cube with normals and/or tex_coords not impl yet.");

    return make_vao(material);
}

}  // namespace pgre::builtin_meshes