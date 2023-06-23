#pragma once

#include "core/debug/exceptions.hpp"
#include "framebuffer.hpp"
#include "rendering/resources/shader.hpp"
#include "rendering/resources/texture.hpp"
#include "rendering/registers/texture_registry.hpp"

#include <glm/glm.hpp>


class DeferredGeometryPassShader : public Shader {
 public:
  DeferredGeometryPassShader();
};

class GBuffer final : public Framebuffer, public TextureSource {
 public:
  GBuffer(int width, int height);
  explicit GBuffer(ImageSize size) : GBuffer(size.width, size.height) {}
  ~GBuffer() override = default;

  Texture getPositionAOTexture() { return m_positionAoBuffer.asTexture(); }
  Texture getNormalRoughnessTexture() {
    return m_normalRoughnessBuffer.asTexture();
  }
  Texture getAlbedoMetallicTexture() {
    return m_albedoMetallicBuffer.asTexture();
  }
  Texture getEmissionTexture() { return m_emissionBuffer.asTexture(); }

  unsigned int bindTexture(unsigned int t_nextTextureUnit,
                           Shader& t_shader) override;

 private:
  // RGB used for position, alpha used for AO.
  Attachment m_positionAoBuffer{};
  // RGB used for normal, alpha used for roughness.
  Attachment m_normalRoughnessBuffer{};
  // RGB used for albedo, alpha used for metallic.
  Attachment m_albedoMetallicBuffer{};
  // RGB used for emission color, alpha channel unused.
  Attachment m_emissionBuffer{};
};
