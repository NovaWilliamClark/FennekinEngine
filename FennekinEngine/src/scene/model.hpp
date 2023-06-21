#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/glm.hpp>

#include "core/debug/exceptions.hpp"
#include "rendering/resources/shader.hpp"
#include "rendering/resources/texture_map.hpp"
#include "scene/mesh.hpp"

class ModelLoaderException : public QuarkException {
    using QuarkException::QuarkException;
};

struct ModelVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec2 texCoords;
};

class ModelMesh final : public Mesh {
public:
    ModelMesh(std::vector<ModelVertex> vertices, const std::vector<unsigned int>& indices,
              const std::vector<TextureMap>& textureMaps, unsigned int instanceCount = 0);

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

class Model : public Renderable {
public:
    explicit Model(const char* path, unsigned int instanceCount = 0);
    ~Model() override = default;
    void loadInstanceModels(const std::vector<glm::mat4>& models);
    void loadInstanceModels(const glm::mat4* models, unsigned int size);
    void drawWithTransform(const glm::mat4& transform, Shader& shader,
                           TextureRegistry* textureRegistry = nullptr) override;

private:
    void loadModel(std::string path);
    void processNode(RenderableNode& target, aiNode* node, const aiScene* scene);
    std::unique_ptr<ModelMesh> processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<TextureMap> loadMaterialTextureMaps(aiMaterial* material, ETextureMapType type);

    unsigned int m_instanceCount;
    RenderableNode m_rootNode;
    std::string m_directory;
    std::unordered_map<std::string, TextureMap> m_loadedTextureMaps;
};
