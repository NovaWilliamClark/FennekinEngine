#include "gBuffer.hpp"

DeferredGeometryPassShader::DeferredGeometryPassShader()
    : Shader(ShaderPath("content/shaders/builtin/deferred.vert"),
             ShaderPath("content/shaders/builtin/deferred.frag")) {}

GBuffer::GBuffer(int width, int height) : Framebuffer(width, height) {
  // Need to use a zero clear color, or else the G-Buffer won't work properly.
  setClearColor(glm::vec4(0.0f));
  // Create and attach all components of the G-Buffer.
  // Don't need to read from depth, so we attach as a renderbuffer.
  attachRenderbuffer(EBufferType::DEPTH_AND_STENCIL);
  // Position and normal are stored as "HDR" colors for higher precision.
  // TODO: Positions can be un-projected from depth without storing them.
  m_positionAoBuffer = attachTexture(EBufferType::COLOR_HDR_ALPHA);
  m_normalRoughnessBuffer = attachTexture(EBufferType::COLOR_SNORM_ALPHA);
  m_albedoMetallicBuffer = attachTexture(EBufferType::COLOR_ALPHA);
  m_emissionBuffer = attachTexture(EBufferType::COLOR_ALPHA);
}

unsigned int GBuffer::bindTexture(const unsigned int t_nextTextureUnit,
                                  Shader& t_shader) {
  m_positionAoBuffer.asTexture().bindToUnit(t_nextTextureUnit + 0);
  m_normalRoughnessBuffer.asTexture().bindToUnit(t_nextTextureUnit + 1);
  m_albedoMetallicBuffer.asTexture().bindToUnit(t_nextTextureUnit + 2);
  m_emissionBuffer.asTexture().bindToUnit(t_nextTextureUnit + 3);
  // Bind sampler uniforms.
  t_shader.setInt("gPositionAO", t_nextTextureUnit + 0);
  t_shader.setInt("gNormalRoughness", t_nextTextureUnit + 1);
  t_shader.setInt("gAlbedoMetallic", t_nextTextureUnit + 2);
  t_shader.setInt("gEmission", t_nextTextureUnit + 3);

  return t_nextTextureUnit + 4;
}