#include "mesh.hpp"

void RenderableNode::drawWithTransform(const glm::mat4& t_transform,
                                       Shader& t_shader,
                                       TextureRegistry* t_textureRegistry) {
  // Combined incoming transform with the node's.
  const glm::mat4 mat = t_transform * getModelTransform();
  for (const auto& renderable : renderables) {
    renderable->drawWithTransform(mat, t_shader, t_textureRegistry);
  }

  // Render children.
  for (const auto& childNode : childNodes) {
    childNode->drawWithTransform(mat, t_shader, t_textureRegistry);
  }
}

void RenderableNode::visitRenderables(const std::function<void(Renderable*)>& t_visitor) const {
  for (auto& renderable : renderables) {
    t_visitor(renderable.get());
  }
  for (auto& childNode : childNodes) {
    childNode->visitRenderables(t_visitor);
  }
}

void Mesh::loadMeshData(const void* t_vertexData, const unsigned int t_numVertices, const unsigned int t_vertexSizeBytes,
                        const std::vector<unsigned int>& t_indices,
                        const std::vector<TextureMap>& t_textureMaps, const unsigned int t_instanceCount) {
 indices = t_indices;
 textureMaps = t_textureMaps;
 numVertices = t_numVertices;
 vertexSizeBytes = t_vertexSizeBytes;
 instanceCount = t_instanceCount;

  // Load VBO.
  vertexArray.loadVertexData(t_vertexData, t_numVertices * t_vertexSizeBytes);

  initializeVertexAttributes();
  initializeVertexArrayInstanceData();

  // Load EBO if this is an indexed mesh.
  if (!t_indices.empty()) {
    vertexArray.loadElementData(t_indices.data(),
                                t_indices.size() * sizeof(unsigned int));
  }
}

void Mesh::loadInstanceModels(const std::vector<glm::mat4>& t_models) {
  vertexArray.loadInstanceVertexData(t_models.data(),
                                     t_models.size() * sizeof(glm::mat4));
}

void Mesh::loadInstanceModels(const glm::mat4* t_models, const unsigned int t_size) {
  vertexArray.loadInstanceVertexData(&t_models[0], t_size * sizeof(glm::mat4));
}

void Mesh::drawWithTransform(const glm::mat4& t_transform, Shader& t_shader,
                             TextureRegistry* t_textureRegistry) {
  // First we set the model transform, combining with the incoming transform.
  t_shader.setMat4("model", t_transform * getModelTransform());

  bindTextures(t_shader, t_textureRegistry);

  // Draw using the VAO.
  t_shader.activate();
  vertexArray.activate();

  glDraw();

  vertexArray.deactivate();

  // Reset.
  t_shader.deactivate();
}

void Mesh::initializeVertexArrayInstanceData() {
  if (instanceCount) {
    // Allocate space for mat4 model transforms for the instancing.
    vertexArray.allocateInstanceVertexData(instanceCount * sizeof(glm::mat4));
    // Add vertex attributes (max attribute size is vec4, so we need 4 of them).
    vertexArray.addVertexAttrib(4, GL_FLOAT, 1);
    vertexArray.addVertexAttrib(4, GL_FLOAT, 1);
    vertexArray.addVertexAttrib(4, GL_FLOAT, 1);
    vertexArray.addVertexAttrib(4, GL_FLOAT, 1);
    vertexArray.finalizeVertexAttribs();
  }
}

void Mesh::bindTextures(Shader& t_shader, TextureRegistry* t_textureRegistry) {
  unsigned int diffuseIdx = 0;
  unsigned int specularIdx = 0;
  unsigned int roughnessIdx = 0;
  unsigned int metallicIdx = 0;
  unsigned int aoIdx = 0;
  unsigned int emissionIdx = 0;
  bool hasNormalMap = false;

  // If a TextureRegistry isn't provided, just start with texture unit 0.
  unsigned int textureUnit = 0;
  if (t_textureRegistry != nullptr) {
    t_textureRegistry->pushUsageBlock();
    textureUnit = t_textureRegistry->getNextTextureUnit();
  }
  for (TextureMap& textureMap : textureMaps) {
    std::string samplerName;
    const ETextureMapType type = textureMap.getType();
    Texture& texture = textureMap.getTexture();
    if (type == ETextureMapType::CUBEMAP) {
      texture.bindToUnit(textureUnit, ETextureBindType::CUBEMAP);
      samplerName = "skybox";
    } else {
      texture.bindToUnit(textureUnit, ETextureBindType::TEXTURE_2D);
      static std::string materialName = "material";
      std::ostringstream ss;
      // TODO: Make this more configurable / less generic?
      ss << "material.";

      // A subset of texture types can be packed into a single texture, which we
      // set a uniform for.
      switch (type) {
        case ETextureMapType::DIFFUSE:
          ss << "diffuseMaps[" << diffuseIdx << "]";
          diffuseIdx++;
          break;
        case ETextureMapType::SPECULAR:
          ss << "specularMaps[" << specularIdx << "]";
          specularIdx++;
          break;
        case ETextureMapType::ROUGHNESS:
          ss << "roughnessMaps[" << roughnessIdx << "]";
          t_shader.setBool(materialName + ".roughnessIsPacked[" +
                             std::to_string(roughnessIdx) + "]",
                         textureMap.isPacked());
          roughnessIdx++;
          break;
        case ETextureMapType::METALLIC:
          ss << "metallicMaps[" << metallicIdx << "]";
          t_shader.setBool(materialName + ".metallicIsPacked[" +
                             std::to_string(metallicIdx) + "]",
                         textureMap.isPacked());
          metallicIdx++;
          break;
        case ETextureMapType::AO:
          ss << "aoMaps[" << aoIdx << "]";
          t_shader.setBool(
              materialName + ".aoIsPacked[" + std::to_string(aoIdx) + "]",
              textureMap.isPacked());
          aoIdx++;
          break;
        case ETextureMapType::EMISSION:
          ss << "emissionMaps[" << emissionIdx << "]";
          emissionIdx++;
          break;
        case ETextureMapType::NORMAL:
          // Only a single normal map supported.
          ss << "normalMap";
          hasNormalMap = true;
          break;
        case ETextureMapType::CUBEMAP:
          // Handled earlier.
          abort();
          break;
      }
      samplerName = ss.str();
    }
    // Set the sampler to the correct texture unit.
    t_shader.setInt(samplerName, textureUnit);

    if (t_textureRegistry != nullptr) {
      textureUnit = t_textureRegistry->getNextTextureUnit();
    } else {
      textureUnit++;
    }
  }
  if (t_textureRegistry != nullptr) {
    t_textureRegistry->popUsageBlock();
  }
  t_shader.setInt("material.diffuseCount", diffuseIdx);
  t_shader.setInt("material.specularCount", specularIdx);
  t_shader.setInt("material.roughnessCount", roughnessIdx);
  t_shader.setInt("material.metallicCount", metallicIdx);
  t_shader.setInt("material.aoCount", aoIdx);
  t_shader.setInt("material.emissionCount", emissionIdx);
  t_shader.setInt("material.hasNormalMap", hasNormalMap);
}

void Mesh::glDraw() {
  // Handle instancing.
  if (instanceCount) {
    // Handle indexed arrays.
    if (!indices.empty()) {
      glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT,
                              nullptr, instanceCount);
    } else {
      glDrawArraysInstanced(GL_TRIANGLES, 0, numVertices, instanceCount);
    }

  } else {
    // Handle indexed arrays.
    if (!indices.empty()) {
      glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
    } else {
      glDrawArrays(GL_TRIANGLES, 0, numVertices);
    }
  }
}
