#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/glm.hpp>

#include "rendering/resources/shader.hpp"
#include "rendering/resources/texture_map.hpp"
#include "scene/mesh.hpp"


struct ModelVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec2 texCoords;
};

class ModelMesh final : public Mesh {
public:
    ModelMesh(std::vector<ModelVertex> t_vertices, const std::vector<unsigned int>& t_indices,
              const std::vector<TextureMap>& t_textureMaps, unsigned int t_instanceCount = 0);

    ~ModelMesh() override = default;

private:
    void initializeVertexAttributes() override;
    std::vector<ModelVertex> m_vertices;
};

constexpr auto DEFAULT_LOAD_FLAGS = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace |
                                    aiProcess_GenUVCoords | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType |
                                    aiProcess_ImproveCacheLocality | aiProcess_OptimizeMeshes |
                                    aiProcess_OptimizeGraph | aiProcess_SplitLargeMeshes |
                                    aiProcess_RemoveRedundantMaterials;

class Model final : public Renderable {
public:
    explicit Model(const char* t_path, unsigned int t_instanceCount = 0);
    ~Model() override = default;
    void loadInstanceModels(const std::vector<glm::mat4>& t_models) const;
    void loadInstanceModels(const glm::mat4* t_models, unsigned int t_size) const;
    void drawWithTransform(const glm::mat4& t_transform, Shader& t_shader,
                           TextureRegistry* t_textureRegistry = nullptr) override;

private:
    void loadModel(const std::string& t_path);
    void processNode(RenderableNode& t_target, const aiNode* t_node, const aiScene* t_scene);
    std::unique_ptr<ModelMesh> processMesh(aiMesh* t_mesh, const aiScene* t_scene);
    std::vector<TextureMap> loadMaterialTextureMaps(const aiMaterial* t_material, ETextureMapType t_type);

    unsigned int m_instanceCount;
    RenderableNode m_rootNode;
    std::string m_directory;
    std::unordered_map<std::string, TextureMap> m_loadedTextureMaps;
};
