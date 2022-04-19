#include <filesystem>
#include <scene/scene.h>

#include <components/all_components.h>
#include <scene/entity.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

namespace pgre::scene {

scene_t::scene_t() = default;

entity_t scene_t::create_entity(const std::string& name) {
    entity_t entity = {_registry.create(), this};
    entity.add_component<component::tag_t>(name);
    entity.add_component<component::transform_t>();
    entity.add_component<component::hierarchy_t>();
    return entity;
}

std::optional<entity_t> load_from_file (const std::filesystem::path& scene_file) {
    static Assimp::Importer importer;
    importer.SetPropertyInteger(AI_CONFIG_PP_PTV_NORMALIZE, 1);


    const aiScene* scn
      = importer.ReadFile(scene_file.c_str(), 0 | aiProcess_Triangulate | aiProcess_GenSmoothNormals
                                                | aiProcess_JoinIdenticalVertices);

    // abort if the loader fails
    if (scn == NULL) {
        spdlog::error("Scene/object failed: {}", importer.GetErrorString());
        return std::nullopt;
    }

    // in this phase we know we have one mesh in our loaded scene, we can directly copy its data to
    // OpenGL ...
    // const aiMesh* mesh = scn->mMeshes[0];

    // *geometry = new MeshGeometry;

    // // vertex buffer object, store all vertex positions and normals
    // glGenBuffers(1, &((*geometry)->vertexBufferObject));
    // glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);
    // glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float) * mesh->mNumVertices, 0,
    //              GL_STATIC_DRAW); // allocate memory for vertices, normals, and texture coordinates
    // // first store all vertices
    // glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sizeof(float) * mesh->mNumVertices, mesh->mVertices);
    // // then store all normals
    // glBufferSubData(GL_ARRAY_BUFFER, 3 * sizeof(float) * mesh->mNumVertices,
    //                 3 * sizeof(float) * mesh->mNumVertices, mesh->mNormals);

    // // just texture 0 for now
    // float* textureCoords = new float[2 * mesh->mNumVertices]; // 2 floats per vertex
    // float* currentTextureCoord = textureCoords;

    // // copy texture coordinates
    // aiVector3D vect;

    // if (mesh->HasTextureCoords(0)) {
    //     // we use 2D textures with 2 coordinates and ignore the third coordinate
    //     for (unsigned int idx = 0; idx < mesh->mNumVertices; idx++) {
    //         vect = (mesh->mTextureCoords[0])[idx];
    //         *currentTextureCoord++ = vect.x;
    //         *currentTextureCoord++ = vect.y;
    //     }
    // }

    // // finally store all texture coordinates
    // glBufferSubData(GL_ARRAY_BUFFER, 6 * sizeof(float) * mesh->mNumVertices,
    //                 2 * sizeof(float) * mesh->mNumVertices, textureCoords);

    // // copy all mesh faces into one big array (assimp supports faces with ordinary number of
    // // vertices, we use only 3 -> triangles)
    // unsigned int* indices = new unsigned int[mesh->mNumFaces * 3];
    // for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {
    //     indices[f * 3 + 0] = mesh->mFaces[f].mIndices[0];
    //     indices[f * 3 + 1] = mesh->mFaces[f].mIndices[1];
    //     indices[f * 3 + 2] = mesh->mFaces[f].mIndices[2];
    // }

    // // copy our temporary index array to OpenGL and free the array
    // glGenBuffers(1, &((*geometry)->elementBufferObject));
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*geometry)->elementBufferObject);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(unsigned) * mesh->mNumFaces, indices,
    //              GL_STATIC_DRAW);

    // delete[] indices;

    // // copy the material info to MeshGeometry structure
    // const aiMaterial* mat = scn->mMaterials[mesh->mMaterialIndex];
    // aiColor4D color;
    // aiString name;
    // aiReturn retValue = AI_SUCCESS;

    // // Get returns: aiReturn_SUCCESS 0 | aiReturn_FAILURE -1 | aiReturn_OUTOFMEMORY -3
    // mat->Get(AI_MATKEY_NAME,
    //          name); // may be "" after the input mesh processing. Must be aiString type!

    // if ((retValue = aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &color)) != AI_SUCCESS)
    //     color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);

    // glGenVertexArrays(1, &((*geometry)->vertexArrayObject));
    // glBindVertexArray((*geometry)->vertexArrayObject);

    // glBindBuffer(
    //   GL_ELEMENT_ARRAY_BUFFER,
    //   (*geometry)->elementBufferObject); // bind our element array buffer (indices) to vao
    // glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);

    // glEnableVertexAttribArray(shader.posLocation);
    // glVertexAttribPointer(shader.posLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // glDisableVertexAttribArray(shader.colorLocation);
    // // following line is problematic on AMD/ATI graphic cards
    // // -> if you see black screen (no objects at all) than try to set color manually in vertex
    // // shader to see at least something
    // glVertexAttrib3f(shader.colorLocation, color.r, color.g, color.b);
    // CHECK_GL_ERROR();

    // glBindVertexArray(0);

    // (*geometry)->numTriangles = mesh->mNumFaces;

    // return true;
}

}  // namespace pgre::scene
