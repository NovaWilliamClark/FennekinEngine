#include "bloom.h"

BloomBuffer::BloomBuffer(int t_width, int t_height) : Framebuffer(t_width, t_height) {
  // Create and attach the bloom buffer. Don't need a depth buffer.
  TextureParams resampleParams = {
      .filtering = ETextureFiltering::BILINEAR,
      .wrapMode = TextureWrapMode::CLAMP_TO_EDGE,
      .generateMips = EMipGeneration::ALWAYS};
  m_bloomMipChainTexture =
      attachTexture(EBufferType::COLOR_HDR_ALPHA, resampleParams);
}

void BloomBuffer::selectMip(int t_mipLevel) {
  if (t_mipLevel < 0 || t_mipLevel >= m_bloomMipChainTexture.numMips) {
    throw BloomException("ERROR::BLOOM::SOURCE_MIP_OUT_OF_RANGE");
  }
  m_bloomMipChainTexture.asTexture().setSamplerMipRange(t_mipLevel, t_mipLevel);
}

void BloomBuffer::deselectMip() {
  m_bloomMipChainTexture.asTexture().unsetSamplerMipRange();
}

unsigned int BloomBuffer::bindTexture(unsigned int t_nextTextureUnit,
                                      Shader& t_shader) {
  m_bloomMipChainTexture.asTexture().bindToUnit(t_nextTextureUnit);
  // Bind sampler uniforms.
  t_shader.setInt("fnk_bloomMipChain", t_nextTextureUnit);

  return t_nextTextureUnit + 1;
}

BloomDownsampleShader::BloomDownsampleShader()
    : ScreenShader(
          ShaderPath("content/shaders/builtin/bloom_downsample.frag")) {}

void BloomDownsampleShader::configureWith(BloomBuffer& t_buffer) {
  // The bloom shader only needs a single texture, so we just bind it
  // directly.
  t_buffer.getBloomMipChainTexture().bindToUnit(0);
  setInt("fnk_bloomMipChain", 0);
}

BloomUpsampleShader::BloomUpsampleShader()
    : ScreenShader(ShaderPath("content/shaders/builtin/bloom_upsample.frag")) {
  setFilterRadius(filterRadius_);
}

void BloomUpsampleShader::configureWith(BloomBuffer& t_buffer) {
  // The bloom shader only needs a single texture, so we just bind it
  // directly.
  t_buffer.getBloomMipChainTexture().bindToUnit(0);
  setInt("fnk_bloomMipChain", 0);
}

BloomPass::BloomPass(int width, int height) : m_bloomBuffer(width, height) {}

void BloomPass::multipassDraw(Framebuffer& sourceFb) {
  // Copy to mip level 0.
  m_bloomBuffer.activate(0);
  sourceFb.blit(m_bloomBuffer, GL_COLOR_BUFFER_BIT);

  int numMips = m_bloomBuffer.getNumMips();

  // Perform the downsampling across the mip chain.
  m_downsampleShader.configureWith(m_bloomBuffer);
  for (int destMip = 1; destMip < numMips; ++destMip) {
    m_bloomBuffer.activate(destMip);
    int sourceMip = destMip - 1;
    m_bloomBuffer.selectMip(sourceMip);
    m_screenQuad.draw(m_downsampleShader);
  }

  // Perform the upsampling, starting with the second-to-last mip. We enable
  // additive blending to avoid having to render into a separate texture.
  m_bloomBuffer.enableAdditiveBlending();
  m_upsampleShader.configureWith(m_bloomBuffer);
  for (int destMip = numMips - 2; destMip >= 0; --destMip) {
    m_bloomBuffer.activate(destMip);
    int sourceMip = destMip + 1;
    m_bloomBuffer.selectMip(sourceMip);
    m_screenQuad.draw(m_upsampleShader);
  }

  m_bloomBuffer.deselectMip();
  m_bloomBuffer.disableAdditiveBlending();
  m_bloomBuffer.deactivate();
}

unsigned int BloomPass::bindTexture(unsigned int t_nextTextureUnit,
                                    Shader& t_shader) {
  getOutput().bindToUnit(t_nextTextureUnit);
  // Bind sampler uniforms.
  t_shader.setInt("fnk_bloom", t_nextTextureUnit);

  return t_nextTextureUnit + 1;
}