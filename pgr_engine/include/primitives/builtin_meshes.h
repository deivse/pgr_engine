#pragma once
#include "vertex_array.h"

namespace pgre::builtin_meshes{

std::shared_ptr<primitives::vertex_array_t>
  get_cube_vao(const std::shared_ptr<material_t>& material, bool normals = false,
               bool tex_coords = false);

}  // namespace pgre::builtin_meshes