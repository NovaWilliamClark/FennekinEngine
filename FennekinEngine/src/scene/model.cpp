#include <iostream>

#include "model.hpp"
#include <gl/glew.h>

#include "core/debug/logger.hpp"

#include <assimp/Importer.hpp>


constexpr ETextureMapType loaderSupportedTextureMapTypes[] = {
    ETextureMapType::DIFFUSE, ETextureMapType::SPECULAR, ETextureMapType::ROUGHNESS, ETextureMapType::METALLIC,
    ETextureMapType::AO,      ETextureMapType::EMISSION, ETextureMapType::NORMAL,
};

glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4& m) {
    // clang-format off
  return glm::mat4(
      m.a1, m.b1, m.c1, m.d1,
      m.a2, m.b2, m.c2, m.d2,
      m.a3, m.b3, m.c3, m.d3,
      m.a4, m.b4, m.c4, m.d4
  );
    // clang-format on
}

ModelMesh::ModelMesh(std::vector<ModelVertex> t_vertices, const std::vector<unsigned int>& t_indices,
                     const std::vector<TextureMap>& t_textureMaps, unsigned int t_instanceCount) :
    m_vertices(std::move(t_vertices)) {
    Mesh::loadMeshData(m_vertices.data(), m_vertices.size(), sizeof(ModelVertex), t_indices, t_textureMaps, t_instanceCount);
}

void ModelMesh::initializeVertexAttributes() {
    // Positions.
    vertexArray.addVertexAttrib(3, GL_FLOAT);
    // Normals.
    vertexArray.addVertexAttrib(3, GL_FLOAT);
    // Tangents.
    vertexArray.addVertexAttrib(3, GL_FLOAT);
    // Texture coordinates.
    vertexArray.addVertexAttrib(2, GL_FLOAT);

    vertexArray.finalizeVertexAttribs();
}

Model::Model(const char* t_path, unsigned int t_instanceCount) : m_instanceCount(t_instanceCount) {
    const std::string pathString(t_path);
    const size_t i = pathString.find_last_of("/");
    // This will either be the model's directory, or empty string if the model is
    // at project root.
    m_directory = i != std::string::npos ? pathString.substr(0, i) : "";

    loadModel(pathString);
}

void Model::loadInstanceModels(const std::vector<glm::mat4>& t_models) const {
    m_rootNode.visitRenderables([&](Renderable* t_renderable) {
        // All renderables in a Model are ModelMeshes.
        auto* mesh = dynamic_cast<ModelMesh*>(t_renderable);
        mesh->loadInstanceModels(t_models);
    });
}

void Model::loadInstanceModels(const glm::mat4* t_models, unsigned int t_size) const {
    m_rootNode.visitRenderables([&](Renderable* t_renderable) {
        // All renderables in a Model are ModelMeshes.
        auto* mesh = dynamic_cast<ModelMesh*>(t_renderable);
        mesh->loadInstanceModels(t_models, t_size);
    });
}

void Model::drawWithTransform(const glm::mat4& t_transform, Shader& t_shader, TextureRegistry* t_textureRegistry) {
    m_rootNode.drawWithTransform(t_transform * getModelTransform(), t_shader, t_textureRegistry);
}

void Model::loadModel(const std::string& t_path) {
    Assimp::Importer importer;
    // Scene is freed by the importer.
    const aiScene* scene = importer.ReadFile(t_path, DEFAULT_LOAD_FLAGS);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        LOG_ERROR(importer.GetErrorString());
    }

    processNode(m_rootNode, scene->mRootNode, scene);
}

void Model::processNode(RenderableNode& t_target, const aiNode* t_node, const aiScene* t_scene) {
    // Consume the transform.
    t_target.setModelTransform(aiMatrix4x4ToGlm(t_node->mTransformation));

    // Process each mesh in the node.
    for (unsigned int i = 0; i < t_node->mNumMeshes; i++) {
        // TODO: This might be creating meshes multiple times when they are
        // referenced by multiple nodes.
        aiMesh* mesh = t_scene->mMeshes[t_node->mMeshes[i]];
        t_target.addRenderable(processMesh(mesh, t_scene));
    }

    // Recurse for children. Recursion stops when no children left.
    for (unsigned int i = 0; i < t_node->mNumChildren; i++) {
        auto childTarget = std::make_unique<RenderableNode>();
        processNode(*childTarget, t_node->mChildren[i], t_scene);
        t_target.addChildNode(std::move(childTarget));
    }
}

std::unique_ptr<ModelMesh> Model::processMesh(aiMesh* t_mesh, const aiScene* t_scene) {
    std::vector<ModelVertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<TextureMap> textureMaps;

    for (unsigned int i = 0; i < t_mesh->mNumVertices; i++) {
        ModelVertex vertex{};

        // Process vertex positions, normals, tangents, and texture coordinates.
        auto inputPos = t_mesh->mVertices[i];
        glm::vec3 position(inputPos.x, inputPos.y, inputPos.z);
        vertex.position = position;

        if (t_mesh->HasNormals()) {
            auto inputNorm = t_mesh->mNormals[i];
            vertex.normal = glm::vec3(inputNorm.x, inputNorm.y, inputNorm.z);
        } else {
            vertex.normal = glm::vec3(0.0f);
        }

        if (t_mesh->HasTangentsAndBitangents()) {
            auto inputTangent = t_mesh->mTangents[i];
            vertex.tangent = glm::vec3(inputTangent.x, inputTangent.y, inputTangent.z);
        } else {
            vertex.tangent = glm::vec3(0.0f);
        }

        // TODO: This is only using the first texture coord set.
        if (t_mesh->HasTextureCoords(0)) {
            auto inputTexCoords = t_mesh->mTextureCoords[0][i];
            vertex.texCoords = glm::vec2(inputTexCoords.x, inputTexCoords.y);
        } else {
            vertex.texCoords = glm::vec2(0.0f);
        }

        vertices.push_back(vertex);
    }

    // Process indices.
    for (unsigned int i = 0; i < t_mesh->mNumFaces; i++) {
        aiFace face = t_mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // Process material.
    {
        aiMaterial* material = t_scene->mMaterials[t_mesh->mMaterialIndex];
        for (auto type : loaderSupportedTextureMapTypes) {
            auto loadedMaps = loadMaterialTextureMaps(material, type);
            textureMaps.insert(textureMaps.end(), loadedMaps.begin(), loadedMaps.end());
        }
    }

    return std::make_unique<ModelMesh>(vertices, indices, textureMaps, m_instanceCount);
}

std::vector<TextureMap> Model::loadMaterialTextureMaps(const aiMaterial* t_material, const ETextureMapType t_type) {
    const std::vector<aiTextureType> aiTypes = textureMapTypeToAiTextureTypes(t_type);
    std::vector<TextureMap> textureMaps;

    for (const aiTextureType aiType : aiTypes) {
        for (unsigned int i = 0; i < t_material->GetTextureCount(aiType); i++) {
            aiString texturePath;
            t_material->GetTexture(aiType, i, &texturePath);
            // TODO: Pull the texture loading bits into a separate class.
            // Assume that the texture path is relative to model directory.
            std::string fullPath = m_directory + "/" + texturePath.C_Str();

            // Don't re-load a texture if it's already been loaded.
            auto item = m_loadedTextureMaps.find(fullPath);
            if (item != m_loadedTextureMaps.end()) {
                // Texture has already been loaded, but likely of a different map type
                // (for example, it could be a combined roughness / metallic map). If
                // so, mark it as a packed texture.
                TextureMap textureMap(item->second.getTexture(), t_type);
                if (t_type != item->second.getType()) {
                    textureMap.setPacked(true);
                    item->second.setPacked(true);
                }
                textureMaps.push_back(textureMap);
                continue;
            }

            // Assume that diffuse and emissive textures are in sRGB.
            // TODO: Allow for a way to override this if necessary.
            const bool isSRGB = t_type == ETextureMapType::DIFFUSE || t_type == ETextureMapType::EMISSION;

            const Texture texture = Texture::load(fullPath.c_str(), isSRGB);
            TextureMap textureMap(texture, t_type);
            m_loadedTextureMaps.insert(std::make_pair(fullPath, textureMap));
            textureMaps.push_back(textureMap);
        }
    }
    return textureMaps;
}
