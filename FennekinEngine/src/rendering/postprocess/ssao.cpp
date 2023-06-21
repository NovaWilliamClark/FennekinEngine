#include "ssao.hpp"
#include "utilities/utils.hpp"

#include <glm/gtx/norm.hpp>
#include <random>

SsaoShader::SsaoShader()
    : Shader(ShaderPath("content/shaders/builtin/screen_quad.vert"),
             ShaderPath("content/shaders/builtin/ssao.frag")) {}

SsaoKernel::SsaoKernel(float t_radius, float t_bias, int t_kernelSize,
                       int t_noiseTextureSideLength)
    : m_radius(t_radius), m_bias(t_bias) {
  regenerate(t_kernelSize, t_noiseTextureSideLength);
}

void SsaoKernel::regenerate(int kernelSize, int noiseTextureSideLength) {
  // Generate the kernel.
  m_kernel.resize(kernelSize);
  for (int i = 0; i < kernelSize; ++i) {
    // Generate a hemisphere sample, with the normal vector pointing in the
    // positive Z direction.

    // First we generate a vector along the sample space.
    glm::vec3 sample(m_rand.next() * 2.0f - 1.0f, m_rand.next() * 2.0f - 1.0f,
                     m_rand.next());
    // Reject samples outside the sphere, to avoid over-sampling the corners.
    if (glm::length2(sample) >= 1.0f) {
      --i;
      continue;
    }

    // Use the vector to generate a point in the hemisphere.
    sample = glm::normalize(sample) * m_rand.next();

    // At this point we have a random point sampled in the hemisphere, but we
    // want to sample more points closer to the actual fragment, so we scale the
    // result.
    float scale = static_cast<float>(i) / kernelSize;
    scale = lerp(0.1f, 1.0f, scale * scale);
    sample *= scale;

    m_kernel[i] = sample;
  }

  // Generate the noise texture used to rotate the kernel.
  std::vector<glm::vec3> noiseData;
  int noiseDataSize = noiseTextureSideLength * noiseTextureSideLength;
  noiseData.resize(noiseDataSize);
  for (int i = 0; i < noiseDataSize; ++i) {
    // Generate a vector on the XY normal plane which we'll use to randomly
    // "tilt" the sample hemisphere in the shader.
    glm::vec3 noise(m_rand.next() * 2.0f - 1.0f, m_rand.next() * 2.0f - 1.0f,
                    0.0f);
    noiseData[i] = noise;
  }

  // Generate the texture. We don't want texture filtering, and we must enable
  // repeat wrap mode so that it properly tiles over the screen.
  TextureParams params = {
      .filtering = ETextureFiltering::NEAREST,
      .wrapMode = TextureWrapMode::REPEAT,
  };
  noiseTexture_ =
      Texture::createFromData(noiseTextureSideLength, noiseTextureSideLength,
                              /*internalFormat=*/GL_RGB16F, noiseData, params);
}

void SsaoKernel::updateUniforms(Shader& shader) {
  shader.setFloat("fnk_ssaoSampleRadius", m_radius);
  shader.setFloat("fnk_ssaoSampleBias", m_bias);
  shader.setInt("fnk_ssaoKernelSize", m_kernel.size());
  for (unsigned int i = 0; i < m_kernel.size(); ++i) {
    shader.setVec3("fnk_ssaoKernel[" + std::to_string(i) + "]", m_kernel[i]);
  }
}

unsigned int SsaoKernel::bindTexture(unsigned int nextTextureUnit,
                                     Shader& shader) {
  noiseTexture_.bindToUnit(nextTextureUnit);
  // Bind sampler uniforms.
  shader.setInt("fnk_ssaoNoise", nextTextureUnit);

  return nextTextureUnit + 1;
}

SsaoBuffer::SsaoBuffer(int width, int height) : Framebuffer(width, height) {
  // Make sure we're clearing properly.
  setClearColor(glm::vec4(0.0f));
  // Create and attach the SSAO buffer. Don't need a depth buffer.
  m_ssaoBuffer = attachTexture(EBufferType::GRAYSCALE);
}

unsigned int SsaoBuffer::bindTexture(unsigned int nextTextureUnit,
                                     Shader& shader) {
  m_ssaoBuffer.asTexture().bindToUnit(nextTextureUnit);
  // Bind sampler uniforms.
  shader.setInt("fnk_ssao", nextTextureUnit);

  return nextTextureUnit + 1;
}

SsaoBlurShader::SsaoBlurShader()
    : Shader(ShaderPath("content/shaders/builtin/screen_quad.vert"),
             ShaderPath("content/shaders/builtin/ssao_blur.frag")) {}

void SsaoBlurShader::configureWith(SsaoKernel& t_kernel, SsaoBuffer& t_buffer) {
  setInt("fnk_ssaoNoiseTextureSideLength", t_kernel.getNoiseTextureSideLength());

  // The blur shader only needs a single texture, so we just bind it directly.
  t_buffer.getSsaoTexture().bindToUnit(0);
  setInt("fnk_ssao", 0);
}