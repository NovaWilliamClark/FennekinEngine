#pragma once

#include "rendering/resources/shader.hpp"
#include "rendering/resources/shader_primitives.hpp"
#include "rendering/core/framebuffer.hpp"
#include "rendering/registers/texture_registry.hpp"

#include "scene/mesh_primitives.hpp"

// A helper for rendering to a cubemap texture in a framebuffer. The passed-in
// framebuffer must outlive the life of this helper.
class CubemapRenderHelper {
 public:
    explicit CubemapRenderHelper(Framebuffer* buffer) : buffer_(buffer) {}

  void setTargetMip(int mip) { targetMip_ = mip; }

  // Draws with the given shader to each face of the cubemap. This results in 6
  // different draw calls. Shader should be prepared (i.e. necessary textures
  // should either be bound or be in the registry, uniforms should be set, etc).
  void multipassDraw(Shader& shader,
                     TextureRegistry* textureRegistry = nullptr);

 private:
  Framebuffer* buffer_;
  RoomMesh room_;
  int targetMip_ = 0;
};

class EquirectCubemapShader final : public Shader {
 public:
  EquirectCubemapShader();
};

// Converts an equirect texture to a cubemap.
class EquirectCubemapConverter final : public TextureSource {
 public:
  EquirectCubemapConverter(int width, int height, bool generateMips = false);
  explicit EquirectCubemapConverter(ImageSize size, bool generateMips = false)
      : EquirectCubemapConverter(size.width, size.height, generateMips) {}

  ~EquirectCubemapConverter() override = default;

  // Draw onto the allocated cubemap from the given texture as the source.
  void multipassDraw(Texture t_source);

  Texture getCubemap() { return m_cubemap.asTexture(); }

  unsigned int bindTexture(unsigned int t_nextTextureUnit,
                           Shader& t_shader) override;

 private:
  Framebuffer m_buffer;
  Attachment m_cubemap;
  EquirectCubemapShader m_equirectCubemapShader;
  CubemapRenderHelper m_cubemapRenderHelper;
  bool m_generateMips;
};