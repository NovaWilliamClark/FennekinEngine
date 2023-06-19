#include "mesh.h"

void RenderableNode::drawWithTransform(const glm::mat4& transform,
                                       Shader& shader,
                                       TextureRegistry* textureRegistry) {
  // Combined incoming transform with the node's.
  const glm::mat4 mat = transform * getModelTransform();
  for (auto& renderable : renderables) {
    renderable->drawWithTransform(mat, shader, textureRegistry);
  }

  // Render children.
  for (auto& childNode : childNodes) {
    childNode->drawWithTransform(mat, shader, textureRegistry);
  }
}

void RenderableNode::visitRenderables(
    std::function<void(Renderable*)> visitor) {
  for (auto& renderable : renderables) {
    visitor(renderable.get());
  }
  for (auto& childNode : childNodes) {
    childNode->visitRenderables(visitor);
  }
}

void Mesh::loadMeshData(const void* t_vertexData, unsigned int t_numVertices,
                        unsigned int t_vertexSizeBytes,
                        const std::vector<unsigned int>& t_indices,
                        const std::vector<TextureMap>& t_textureMaps,
                        unsigned int t_instanceCount) {
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
    vertexArray.loadElementData(&t_indices[0],
                                 t_indices.size() * sizeof(unsigned int));
  }
}

void Mesh::loadInstanceModels(const std::vector<glm::mat4>& models) {
  vertexArray.loadInstanceVertexData(models.data(),
                                     models.size() * sizeof(glm::mat4));
}

void Mesh::loadInstanceModels(const glm::mat4* models, unsigned int size) {
  vertexArray.loadInstanceVertexData(&models[0], size * sizeof(glm::mat4));
}

void Mesh::drawWithTransform(const glm::mat4& transform, Shader& shader,
                             TextureRegistry* textureRegistry) {
  // First we set the model transform, combining with the incoming transform.
  shader.setMat4("model", transform * getModelTransform());

  bindTextures(shader, textureRegistry);

  // Draw using the VAO.
  shader.activate();
  vertexArray.activate();

  glDraw();

  vertexArray.deactivate();

  // Reset.
  shader.deactivate();
}

void Mesh::initializeVertexArrayInstanceData() {
  if (instanceCount) {
    // Allocate space for mat4 model transforms for the instancing.
    vertexArray.allocateInstanceVertexData(instanceCount * sizeof(glm::mat4));
    // Add vertex attributes (max attribute size is vec4, so we need 4 of them).
    vertexArray.addVertexAttrib(4, GL_FLOAT, /*instanceDivisor=*/1);
    vertexArray.addVertexAttrib(4, GL_FLOAT, /*instanceDivisor=*/1);
    vertexArray.addVertexAttrib(4, GL_FLOAT, /*instanceDivisor=*/1);
    vertexArray.addVertexAttrib(4, GL_FLOAT, /*instanceDivisor=*/1);
    vertexArray.finalizeVertexAttribs();
  }
}

void Mesh::bindTextures(Shader& shader, TextureRegistry* textureRegistry) {
  // Bind textures. Assumes uniform naming is "material.textureMapType[idx]".
  unsigned int diffuseIdx = 0;
  unsigned int specularIdx = 0;
  unsigned int roughnessIdx = 0;
  unsigned int metallicIdx = 0;
  unsigned int aoIdx = 0;
  unsigned int emissionIdx = 0;
  bool hasNormalMap = false;

  // If a TextureRegistry isn't provided, just start with texture unit 0.
  unsigned int textureUnit = 0;
  if (textureRegistry != nullptr) {
    textureRegistry->pushUsageBlock();
    textureUnit = textureRegistry->getNextTextureUnit();
  }
  for (TextureMap& textureMap : textureMaps) {
    std::string samplerName;
    ETextureMapType type = textureMap.getType();
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
          shader.setBool(materialName + ".roughnessIsPacked[" +
                             std::to_string(roughnessIdx) + "]",
                         textureMap.isPacked());
          roughnessIdx++;
          break;
        case ETextureMapType::METALLIC:
          ss << "metallicMaps[" << metallicIdx << "]";
          shader.setBool(materialName + ".metallicIsPacked[" +
                             std::to_string(metallicIdx) + "]",
                         textureMap.isPacked());
          metallicIdx++;
          break;
        case ETextureMapType::AO:
          ss << "aoMaps[" << aoIdx << "]";
          shader.setBool(
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
    shader.setInt(samplerName, textureUnit);

    if (textureRegistry != nullptr) {
      textureUnit = textureRegistry->getNextTextureUnit();
    } else {
      textureUnit++;
    }
  }
  if (textureRegistry != nullptr) {
    textureRegistry->popUsageBlock();
  }
  shader.setInt("material.diffuseCount", diffuseIdx);
  shader.setInt("material.specularCount", specularIdx);
  shader.setInt("material.roughnessCount", roughnessIdx);
  shader.setInt("material.metallicCount", metallicIdx);
  shader.setInt("material.aoCount", aoIdx);
  shader.setInt("material.emissionCount", emissionIdx);
  shader.setInt("material.hasNormalMap", hasNormalMap);
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
