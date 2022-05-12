#include "math/aabb.h"
#include <scene/scene.h>

#include <scene/entity.h>
#include <components/all_components.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>



glm::vec3 vec3_cast(const aiColor3D &v) { return glm::vec3(v.r, v.g, v.b); } 
glm::vec3 vec3_cast(const aiVector3D &v) { return glm::vec3(v.x, v.y, v.z); } 
glm::vec2 vec2_cast(const aiVector3D &v) { return glm::vec2(v.x, v.y); } // it's aiVector3D because assimp's texture coordinates use that
glm::quat quat_cast(const aiQuaternion &q) { return glm::quat(q.w, q.x, q.y, q.z); } 
glm::mat4 mat4_cast(const aiMatrix4x4 &m) { return glm::transpose(glm::make_mat4(&m.a1)); }

namespace pgre::scene {

std::vector<std::shared_ptr<phong_material_t>> import_materials(const aiScene* ai_scene, const std::filesystem::path& scene_file){
    std::vector<std::shared_ptr<phong_material_t>> materials(ai_scene->mNumMaterials, nullptr);
    for (unsigned int i = 0; i < ai_scene->mNumMaterials; i++) {
        aiMaterial& ai_material = *ai_scene->mMaterials[i];
        static aiColor3D ambient{0.0f}, diffuse{0.0f}, specular{0.0f}, transparent{0.0f};
        float shininess{}, transparency{};
        if (ai_material.Get(AI_MATKEY_COLOR_AMBIENT, ambient) != AI_SUCCESS)
            spdlog::error("Import: Failed to get material ambient color.");
        if (ai_material.Get(AI_MATKEY_COLOR_DIFFUSE, diffuse) != AI_SUCCESS)
            spdlog::error("Import: Failed to get material diffuse color.");
        if (ai_material.Get(AI_MATKEY_COLOR_SPECULAR, specular) != AI_SUCCESS)
            spdlog::error("Import: Failed to get material specular color.");
        if (ai_material.Get(AI_MATKEY_SHININESS, shininess) != AI_SUCCESS)
            spdlog::error("Import: Failed to get material shininess.");

        // if (ai_material.Get(AI_MATKEY_OPACITY, transparency)
        //     == AI_SUCCESS) { // TODO: transparency not working
        //     transparency = 1.0f - transparency;
        // } else {
        //     if (ai_material.Get(AI_MATKEY_TRANSPARENCYFACTOR, transparency) != AI_SUCCESS)
        //         spdlog::error("Import: Failed to get material opacity and transparency.");
        // }

        std::shared_ptr<texture2D_t> color_texture{nullptr};
        if (ai_material.GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            aiString path;
            if (ai_material.GetTexture(aiTextureType_DIFFUSE, 0, &path, nullptr, nullptr, nullptr,
                                       nullptr, nullptr)
                == AI_SUCCESS) {
                color_texture = std::make_shared<texture2D_t>(
                  std::filesystem::absolute(scene_file.parent_path()) / path.C_Str());
            } else {
                spdlog::error("Import: failed to get diffuse texture for material.");
            }
        }

        if (color_texture) {
            materials[i] = std::make_shared<phong_material_t>(
              std::move(color_texture), glm::vec3{diffuse.r, diffuse.g, diffuse.b},
              glm::vec3{ambient.r, ambient.g, ambient.b},
              glm::vec3{specular.r, specular.g, specular.b}, shininess, transparency);
        } else {
            materials[i] = std::make_shared<phong_material_t>(
              glm::vec3{diffuse.r, diffuse.g, diffuse.b},
              glm::vec3{ambient.r, ambient.g, ambient.b},
              glm::vec3{specular.r, specular.g, specular.b}, shininess, transparency);
        }
    }
    return materials;
}

std::vector<std::pair<std::shared_ptr<primitives::vertex_array_t>, std::pair<glm::vec3, glm::vec3>>> import_meshes(const aiScene* ai_scene){
    std::vector<std::pair<std::shared_ptr<primitives::vertex_array_t>, std::pair<glm::vec3, glm::vec3>>> vertex_arrays(ai_scene->mNumMeshes);
    for (unsigned int i = 0; i < ai_scene->mNumMeshes; i++) {
        vertex_arrays[i].first = std::make_shared<primitives::vertex_array_t>();
        auto vertex_buffer = std::make_shared<primitives::vertex_buffer_t>();
        auto index_buffer = std::make_shared<primitives::index_buffer_t>();

        auto* ai_mesh = ai_scene->mMeshes[i];

        if (!ai_mesh->HasNormals()) throw std::runtime_error("Mesh has no normals!");
        bool tex_coords = ai_mesh->HasTextureCoords(0);

        /*                            \/ -- position + normals .*/
        uint8_t floats_per_vertex = (6 + (tex_coords ? 2 : 0));
        std::vector<float> interleaved_data(floats_per_vertex * ai_mesh->mNumVertices);

        for (size_t vertex_ix = 0; vertex_ix < ai_mesh->mNumVertices; vertex_ix++) {
            memcpy(interleaved_data.data() + vertex_ix * floats_per_vertex,
                   &ai_mesh->mVertices[vertex_ix], 3 * sizeof(float));
            memcpy(interleaved_data.data() + vertex_ix * floats_per_vertex + 3,
                   &ai_mesh->mNormals[vertex_ix], 3 * sizeof(float)); 
            if (tex_coords) {
                interleaved_data[vertex_ix * floats_per_vertex + 6]
                  = ai_mesh->mTextureCoords[0][vertex_ix].x;
                interleaved_data[vertex_ix * floats_per_vertex + 7]
                  = ai_mesh->mTextureCoords[0][vertex_ix].y;
            }
        }

        vertex_buffer->set_data(interleaved_data.size() * sizeof(float), interleaved_data.data());

        auto layout = primitives::buffer_layout_t( // INTERLEAVED!!!
          tex_coords
            ? std::initializer_list<primitives::buffer_element_t>{{GL_FLOAT, 3, "position"},
                                                                  {GL_FLOAT, 3, "normal"},
                                                                  {GL_FLOAT, 2, "tex_coord"}}
            : std::initializer_list<primitives::buffer_element_t>{{GL_FLOAT, 3, "position"},
                                                                  {GL_FLOAT, 3, "normal"}},
          std::make_shared<phong_material_t>());

        std::vector<GLuint> indices(ai_mesh->mNumFaces * 3);
        for (unsigned int face_ix = 0; face_ix < ai_mesh->mNumFaces; face_ix++) {
            debug_assert(ai_mesh->mFaces[face_ix].mNumIndices == 3,
                         "Sorry to say bro... Non triangular mesh :(");
            memcpy(indices.data() + (face_ix * 3),
                   ai_mesh->mFaces[face_ix].mIndices, 3 * sizeof(GLuint));
        }
        index_buffer->set_data(indices);

        vertex_arrays[i].first->add_vertex_buffer(vertex_buffer, std::make_shared<primitives::buffer_layout_t>(layout));
        vertex_arrays[i].first->set_index_buffer(index_buffer);
        vertex_arrays[i].second = math::calc_aabb(&(ai_scene->mMeshes[i]->mVertices[0].x), ai_scene->mMeshes[i]->mNumVertices);
    }
    return vertex_arrays;
}

void import_lights(const aiScene* ai_scene, scene_t* scene, entt::registry& registry){
    for (unsigned int i = 0; i < ai_scene->mNumLights; i++) {
        auto* ai_light = ai_scene->mLights[i];
        auto named = registry.view<component::tag_t>();
        entt::entity light_owner = entt::null;
        for (auto entity : named) {
            if (registry.get<component::tag_t>(entity).tag == ai_light->mName.C_Str()) {
                light_owner = entity;
                break;
            }
        }
        pgre::debug_assert(light_owner != entt::null, "Each light was supposed to have a node...");
        auto light_entity = entity_t{light_owner, scene};
        switch (ai_light->mType) {
            case aiLightSource_DIRECTIONAL:
                light_entity.add_component<component::sun_light_t>(
                  vec3_cast(ai_light->mColorAmbient), vec3_cast(ai_light->mColorDiffuse),
                  vec3_cast(ai_light->mColorSpecular), vec3_cast(ai_light->mDirection));
                break;
            case aiLightSource_POINT:
                light_entity.add_component<component::point_light_t>(
                  vec3_cast(ai_light->mColorAmbient), vec3_cast(ai_light->mColorDiffuse),
                  vec3_cast(ai_light->mColorSpecular),
                  glm::vec3{ai_light->mAttenuationConstant, ai_light->mAttenuationLinear,
                            ai_light->mAttenuationQuadratic});
                break;
            case aiLightSource_SPOT:
                light_entity.add_component<component::spot_light_t>(
                  vec3_cast(ai_light->mColorAmbient), vec3_cast(ai_light->mColorDiffuse),
                  vec3_cast(ai_light->mColorSpecular), vec3_cast(ai_light->mDirection),
                  ai_light->mAngleOuterCone,
                  0.5f * (ai_light->mAngleOuterCone / ai_light->mAngleInnerCone),
                  glm::vec3{ai_light->mAttenuationConstant, ai_light->mAttenuationLinear,
                            ai_light->mAttenuationQuadratic});
                break;
            default:
                light_entity.destroy();
                spdlog::warn("Trying to import unsupported light type.");
                break;
        }
    }
}

void add_mesh_and_bb_components(
  scene_t& scene, entity_t& target, aiNode* ai_node, std::vector<std::shared_ptr<phong_material_t>>& materials,
  std::vector<std::pair<std::shared_ptr<primitives::vertex_array_t>, std::pair<glm::vec3, glm::vec3>>>& vertex_arrays,
  const aiScene* ai_scene) {

    if (ai_node->mNumMeshes >= 1) {
        auto vao_arr_ix = ai_node->mMeshes[0];

        target.add_component<component::mesh_t>(
          vertex_arrays[vao_arr_ix].first,
          materials[ai_scene->mMeshes[vao_arr_ix]->mMaterialIndex]);
        target.add_component<component::bounding_box_t>(vertex_arrays[vao_arr_ix].second);
    }
    for (auto mesh_ix = 1; mesh_ix < ai_node->mNumMeshes; mesh_ix++) {
        auto sub_entity
          = scene.create_entity(fmt::format("{}__sub_mesh_{}", target.get_name(), mesh_ix));
        target.add_child(sub_entity);
        auto vao_arr_ix = ai_node->mMeshes[mesh_ix];

        sub_entity.add_component<component::mesh_t>(
          vertex_arrays[vao_arr_ix].first,
          materials[ai_scene->mMeshes[vao_arr_ix]->mMaterialIndex]);
        sub_entity.add_component<component::bounding_box_t>(vertex_arrays[vao_arr_ix].second);
    }

}

std::optional<entity_t> scene_t::import_from_file(const std::filesystem::path& scene_file) {
    static Assimp::Importer importer;
    importer.SetPropertyInteger(AI_CONFIG_PP_PTV_NORMALIZE, 1);

    const aiScene* ai_scene
      = importer.ReadFile(scene_file.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals
                                                | aiProcess_GenUVCoords);

    // abort if the loader fails
    if (ai_scene == NULL) {
        spdlog::error("Scene/object failed: {}", importer.GetErrorString());
        return std::nullopt;
    }

    debug_assert(ai_scene->mNumTextures == 0,
                 "Sorry bro, embedded textures - no can do...");

    auto materials = import_materials(ai_scene, scene_file);
    auto vertex_arrays = import_meshes(ai_scene);

    

    // Create entities and scene graph 
    auto* ai_root = ai_scene->mRootNode;
    entity_t root
      = this->create_entity(ai_root->mName.C_Str(), glm::mat4{1});
    if (ai_root->mNumMeshes != 0) {
        root.add_component<component::mesh_t>(
          vertex_arrays[ai_root->mMeshes[0]].first,
          materials[ai_scene->mMeshes[ai_root->mMeshes[0]]->mMaterialIndex]);
        root.add_component<component::bounding_box_t>(vertex_arrays[ai_root->mMeshes[0]].second);
    }

    for (unsigned int child_ix = 0; child_ix < ai_root->mNumChildren; child_ix++) {
        hierarchy_import_rec(root, ai_root->mChildren[child_ix], materials,
                             vertex_arrays, ai_scene);
    }

    // Has to be after hieararchy is created, as we add lights to existing entities.
    import_lights(ai_scene, this, _registry);

    return root;
}

void scene_t::hierarchy_import_rec(
  entity_t& parent, aiNode* ai_node, std::vector<std::shared_ptr<phong_material_t>>& materials,
  std::vector<std::pair<std::shared_ptr<primitives::vertex_array_t>, std::pair<glm::vec3, glm::vec3>>>& vertex_arrays,
  const aiScene* ai_scene) {
    auto transform = mat4_cast(ai_node->mTransformation);
    auto new_entity = this->create_entity(ai_node->mName.C_Str(), transform);
    parent.add_child(new_entity);

    add_mesh_and_bb_components(*this, new_entity, ai_node, materials, vertex_arrays, ai_scene);

    for (unsigned int child_ix = 0; child_ix < ai_node->mNumChildren; child_ix++) {
        hierarchy_import_rec(new_entity, ai_node->mChildren[child_ix], materials,
                             vertex_arrays, ai_scene);
    }
}
} // namespace pgre::scene