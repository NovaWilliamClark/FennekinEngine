#pragma once

#include <functional>

#include <gl/glew.h>

#include "rendering/core/vertex_array.hpp"
#include "rendering/registers/texture_registry.hpp"
#include "rendering/resources/shader.hpp"
#include "rendering/resources/texture_map.hpp"

#include <sstream>
#include <string>
#include <vector>
#include <glm/glm.hpp>


class Renderable {
 public:
  virtual ~Renderable() = default;

  glm::mat4 getModelTransform() const { return model; }
  void setModelTransform(const glm::mat4& t_model) { model = t_model; }

  // TODO: Add translation, rotation, scale methods.

  virtual void draw(Shader& t_shader,
                    TextureRegistry* t_textureRegistry = nullptr) {
      constexpr glm::mat4 transform(1.0f);
    return drawWithTransform(transform, t_shader, t_textureRegistry);
  }
  virtual void drawWithTransform(
      const glm::mat4& t_transform, Shader& t_shader,
      TextureRegistry* t_textureRegistry = nullptr) = 0;

 protected:
  // The model transform matrix.
  glm::mat4 model = glm::mat4(1.0f);
};

// A node in a tree of Renderables, containing one or more renderables.
// When rendering, each node's transform is applied to the transform of its
// Renderables, as well as to its child RenderableNodes.
class RenderableNode : public Renderable {
 public:
    ~RenderableNode() override = default;
  void drawWithTransform(const glm::mat4& t_transform, Shader& t_shader,
                         TextureRegistry* t_textureRegistry = nullptr) override;

  void addRenderable(std::unique_ptr<Renderable> renderable) {
    renderables.push_back(std::move(renderable));
  }

  void addChildNode(std::unique_ptr<RenderableNode> childNode) {
    childNodes.push_back(std::move(childNode));
  }

  void visitRenderables(const std::function<void(Renderable*)>& t_visitor) const;

 protected:
  // The set of Renderables making up this node.
  std::vector<std::unique_ptr<Renderable>> renderables;
  // The set of child RenderableNodes.
  std::vector<std::unique_ptr<RenderableNode>> childNodes;
};

// An abstract class that represents a triangle mesh and handles loading and
// rendering. Child classes can specialize when configuring vertex attributes.
class Mesh : public Renderable {
 public:
    ~Mesh() override = default;

  void loadInstanceModels(const std::vector<glm::mat4>& t_models);
  void loadInstanceModels(const glm::mat4* t_models, unsigned int t_size);
  void drawWithTransform(const glm::mat4& t_transform, Shader& t_shader,
                         TextureRegistry* t_textureRegistry = nullptr) override;

  std::vector<unsigned int> getIndices() { return indices; }
  std::vector<TextureMap> getTextureMaps() { return textureMaps; }

 protected:
  // Loads mesh data into the mesh. Calls initializeVertexAttributes and
  // initializeVertexArrayInstanceData under the hood. Must be called
  // immediately after construction.
  virtual void loadMeshData(const void* t_vertexData, unsigned int t_numVertices,
                            unsigned int t_vertexSizeBytes,
                            const std::vector<unsigned int>& t_indices,
                            const std::vector<TextureMap>& t_textureMaps,
                            unsigned int t_instanceCount = 0);
  // Initializes vertex attributes.
  virtual void initializeVertexAttributes() = 0;
  // Allocates and initializes vertex array instance data.
  virtual void initializeVertexArrayInstanceData();
  // Binds texture maps to texture units and sets shader sampler uniforms.
  virtual void bindTextures(Shader& t_shader, TextureRegistry* t_textureRegistry);
  // Emits glDraw* calls based on the mesh instancing/indexing. Requires shaders
  // and VAOs to be active prior to calling.
  virtual void glDraw();

  VertexArray vertexArray;
  std::vector<unsigned int> indices;
  std::vector<TextureMap> textureMaps;

  // The number of vertices in the mesh.
  unsigned int numVertices = 0;
  // The size, in bytes, of each vertex.
  unsigned int vertexSizeBytes = 0;
  unsigned int instanceCount = 0;
};