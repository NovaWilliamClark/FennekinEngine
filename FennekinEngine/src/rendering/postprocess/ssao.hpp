#pragma once

#include "core/debug/exceptions.hpp"
#include "rendering/core/framebuffer.hpp"
#include "utilities/random.hpp"
#include "rendering/resources/shader.hpp"
#include "rendering/resources/texture.hpp"
#include "rendering/registers/texture_registry.hpp"

#include <glm/glm.hpp>
#include <vector>

class SsaoException : public QuarkException {
  using QuarkException::QuarkException;
};

// A shader that calculates SSAO based on a G-Buffer and SSAO kernel.
class SsaoShader : public Shader {
 public:
  SsaoShader();
};

// A sample kernel for use in screen space ambient occlusion. Uses a hemisphere
// sampling method and a noise texture.
class SsaoKernel : public UniformSource, public TextureSource {
 public:
  SsaoKernel(float t_radius = 0.5, float t_bias = 0.025, int t_kernelSize = 64,
             int t_noiseTextureSideLength = 4);
  int getSize() const { return m_kernel.size(); }
  int getNoiseTextureSideLength() const { return noiseTexture_.getWidth(); }
  float getRadius() const { return m_radius; }
  void setRadius(float radius) { m_radius = radius; }
  float getBias() const { return m_bias; }
  void setBias(float bias) { m_bias = bias; }

  // Binds kernel uniforms.
  void updateUniforms(Shader& shader) override;

  // Binds the noise texture.
  unsigned int bindTexture(unsigned int nextTextureUnit,
                           Shader& shader) override;

 private:
  // TODO: Expose this after texture lifecycle is handled (currently
  // regenerating would cause orphaned textures).
  void regenerate(int kernelSize, int noiseTextureSideLength);

  float m_radius;
  float m_bias;
  std::vector<glm::vec3> m_kernel;
  Texture noiseTexture_;
  UniformRandom m_rand;
};

class SsaoBuffer : public Framebuffer, public TextureSource {
 public:
  SsaoBuffer(int width, int height);
  explicit SsaoBuffer(ImageSize size) : SsaoBuffer(size.width, size.height) {}
  ~SsaoBuffer() override = default;

  Texture getSsaoTexture() { return m_ssaoBuffer.asTexture(); }

  unsigned int bindTexture(unsigned int nextTextureUnit,
                           Shader& shader) override;

 private:
  Attachment m_ssaoBuffer;
};

// A simple shader that blurs a precomputed SSAO buffer.
class SsaoBlurShader : public Shader {
 public:
  SsaoBlurShader();

  // Configures uniforms for the blur to run correctly for the given kernel,
  // using the given buffer as the source image. After this, you can draw onto
  // another separate SsaoBuffer to get the blurred result.
  void configureWith(SsaoKernel& t_kernel, SsaoBuffer& t_buffer);
};