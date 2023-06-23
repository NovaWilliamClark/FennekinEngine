#pragma once

#include "rendering/core/framebuffer.hpp"
#include "rendering/registers/texture_registry.hpp"
#include "rendering/resources/shader.hpp"
#include "scene/mesh_primitives.hpp"

#include "rendering/resources/shader_primitives.hpp"


class BloomBuffer : public Framebuffer, public TextureSource {
 public:
  BloomBuffer(int t_width, int t_height);
  explicit BloomBuffer(const ImageSize t_size) : BloomBuffer(t_size.width, t_size.height) {}
  ~BloomBuffer() override = default;

  Texture getBloomMipChainTexture() {
    return m_bloomMipChainTexture.asTexture();
  }

  int getNumMips() const { return m_bloomMipChainTexture.numMips; }

  // Limits sampling from any other mip other than the given mip. This is
  // important to avoid undefined behavior when drawing to a mip level while
  // sampling from another.
  void selectMip(int t_mipLevel);
  void deselectMip();

  unsigned int bindTexture(unsigned int t_nextTextureUnit,
                           Shader& t_shader) override;

 private:
  Attachment m_bloomMipChainTexture;
};

class BloomDownsampleShader : public ScreenShader {
 public:
  BloomDownsampleShader();

  void configureWith(BloomBuffer& t_buffer);
};

class BloomUpsampleShader : public ScreenShader {
 public:
  BloomUpsampleShader();

  void configureWith(BloomBuffer& t_buffer);
  void setFilterRadius(float filterRadius) {
    filterRadius_ = filterRadius;
    setFloat("fnk_filterRadius", filterRadius_);
  }
  float getFilterRadius() const { return filterRadius_; }

 private:
  static constexpr float DEFAULT_FILTER_RADIUS = 0.005f;
  float filterRadius_ = DEFAULT_FILTER_RADIUS;
};

// A self contained bloom pass that uses a series of down/upsamples to perform
// the bloom effect.
class BloomPass : public TextureSource {
 public:
  BloomPass(int width, int height);
  explicit BloomPass(ImageSize size) : BloomPass(size.width, size.height) {}
  virtual ~BloomPass() = default;

  // Sets the radius, in UV coordinates, for the upscaling kernel.
  void setFilterRadius(float filterRadius) {
    m_upsampleShader.setFilterRadius(filterRadius);
  }
  float getFilterRadius() const { return m_upsampleShader.getFilterRadius(); }

  // Performs the bloom based on the image from a source framebuffer. The
  // currently configured color component is blitted into the bloom buffer,
  // after which the bloom downsample/upsample calls are made.
  void multipassDraw(Framebuffer& sourceFb);

  int getNumMips() const { return m_bloomBuffer.getNumMips(); }
  void selectMip(int mipLevel) { m_bloomBuffer.selectMip(mipLevel); }
  void deselectMip() { m_bloomBuffer.deselectMip(); }

  Texture getOutput() { return m_bloomBuffer.getBloomMipChainTexture(); }

  unsigned int bindTexture(unsigned int t_nextTextureUnit,
                           Shader& t_shader) override;

 private:
  ScreenQuadMesh m_screenQuad;
  BloomBuffer m_bloomBuffer;
  BloomDownsampleShader m_downsampleShader;
  BloomUpsampleShader m_upsampleShader;
};