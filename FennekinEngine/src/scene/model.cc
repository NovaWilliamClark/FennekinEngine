#include <iostream>

#include "model.h"
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

ModelMesh::ModelMesh(std::vector<ModelVertex> vertices, const std::vector<unsigned int>& indices,
                     const std::vector<TextureMap>& textureMaps, unsigned int instanceCount) :
    m_vertices(std::move(vertices)) {
    Mesh::loadMeshData(m_vertices.data(), m_vertices.size(), sizeof(ModelVertex), indices, textureMaps, instanceCount);
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

Model::Model(const char* path, unsigned int instanceCount) : m_instanceCount(instanceCount) {
    const std::string pathString(path);
    const size_t i = pathString.find_last_of("/");
    // This will either be the model's directory, or empty string if the model is
    // at project root.
    m_directory = i != std::string::npos ? pathString.substr(0, i) : "";

    loadModel(pathString);
}

void Model::loadInstanceModels(const std::vector<glm::mat4>& models) {
    m_rootNode.visitRenderables([&](Renderable* t_renderable) {
        // All renderables in a Model are ModelMeshes.
        auto* mesh = dynamic_cast<ModelMesh*>(t_renderable);
        mesh->loadInstanceModels(models);
    });
}

void Model::loadInstanceModels(const glm::mat4* models, unsigned int size) {
    m_rootNode.visitRenderables([&](Renderable* t_renderable) {
        // All renderables in a Model are ModelMeshes.
        auto* mesh = dynamic_cast<ModelMesh*>(t_renderable);
        mesh->loadInstanceModels(models, size);
    });
}

void Model::drawWithTransform(const glm::mat4& transform, Shader& shader, TextureRegistry* textureRegistry) {
    m_rootNode.drawWithTransform(transform * getModelTransform(), shader, textureRegistry);
}

void Model::loadModel(std::string path) {
    Assimp::Importer importer;
    // Scene is freed by the importer.
    const aiScene* scene = importer.ReadFile(path, DEFAULT_LOAD_FLAGS);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        LOG_ERROR(importer.GetErrorString());
    }

    processNode(m_rootNode, scene->mRootNode, scene);
}

void Model::processNode(RenderableNode& target, aiNode* node, const aiScene* scene) {
    // Consume the transform.
    target.setModelTransform(aiMatrix4x4ToGlm(node->mTransformation));

    // Process each mesh in the node.
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        // TODO: This might be creating meshes multiple times when they are
        // referenced by multiple nodes.
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        target.addRenderable(processMesh(mesh, scene));
    }

    // Recurse for children. Recursion stops when no children left.
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        auto childTarget = std::make_unique<RenderableNode>();
        processNode(*childTarget, node->mChildren[i], scene);
        target.addChildNode(std::move(childTarget));
    }
}

std::unique_ptr<ModelMesh> Model::processMesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<ModelVertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<TextureMap> textureMaps;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        ModelVertex vertex{};

        // Process vertex positions, normals, tangents, and texture coordinates.
        auto inputPos = mesh->mVertices[i];
        glm::vec3 position(inputPos.x, inputPos.y, inputPos.z);
        vertex.position = position;

        if (mesh->HasNormals()) {
            auto inputNorm = mesh->mNormals[i];
            vertex.normal = glm::vec3(inputNorm.x, inputNorm.y, inputNorm.z);
        } else {
            vertex.normal = glm::vec3(0.0f);
        }

        if (mesh->HasTangentsAndBitangents()) {
            auto inputTangent = mesh->mTangents[i];
            vertex.tangent = glm::vec3(inputTangent.x, inputTangent.y, inputTangent.z);
        } else {
            vertex.tangent = glm::vec3(0.0f);
        }

        // TODO: This is only using the first texture coord set.
        if (mesh->HasTextureCoords(0)) {
            auto inputTexCoords = mesh->mTextureCoords[0][i];
            vertex.texCoords = glm::vec2(inputTexCoords.x, inputTexCoords.y);
        } else {
            vertex.texCoords = glm::vec2(0.0f);
        }

        vertices.push_back(vertex);
    }

    // Process indices.
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // Process material.
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        for (auto type : loaderSupportedTextureMapTypes) {
            auto loadedMaps = loadMaterialTextureMaps(material, type);
            textureMaps.insert(textureMaps.end(), loadedMaps.begin(), loadedMaps.end());
        }
    }

    return std::make_unique<ModelMesh>(vertices, indices, textureMaps, m_instanceCount);
}

std::vector<TextureMap> Model::loadMaterialTextureMaps(aiMaterial* material, ETextureMapType type) {
    std::vector<aiTextureType> aiTypes = textureMapTypeToAiTextureTypes(type);
    std::vector<TextureMap> textureMaps;

    for (aiTextureType aiType : aiTypes) {
        for (unsigned int i = 0; i < material->GetTextureCount(aiType); i++) {
            aiString texturePath;
            material->GetTexture(aiType, i, &texturePath);
            // TODO: Pull the texture loading bits into a separate class.
            // Assume that the texture path is relative to model directory.
            std::string fullPath = m_directory + "/" + texturePath.C_Str();

            // Don't re-load a texture if it's already been loaded.
            auto item = m_loadedTextureMaps.find(fullPath);
            if (item != m_loadedTextureMaps.end()) {
                // Texture has already been loaded, but likely of a different map type
                // (for example, it could be a combined roughness / metallic map). If
                // so, mark it as a packed texture.
                TextureMap textureMap(item->second.getTexture(), type);
                if (type != item->second.getType()) {
                    textureMap.setPacked(true);
                    item->second.setPacked(true);
                }
                textureMaps.push_back(textureMap);
                continue;
            }

            // Assume that diffuse and emissive textures are in sRGB.
            // TODO: Allow for a way to override this if necessary.
            bool isSRGB = type == ETextureMapType::DIFFUSE || type == ETextureMapType::EMISSION;

            Texture texture = Texture::load(fullPath.c_str(), isSRGB);
            TextureMap textureMap(texture, type);
            m_loadedTextureMaps.insert(std::make_pair(fullPath, textureMap));
            textureMaps.push_back(textureMap);
        }
    }
    return textureMaps;
}
