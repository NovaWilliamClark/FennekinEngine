#include "texture_registry.hpp"

void TextureRegistry::updateUniforms(Shader& t_shader) {
  m_nextTextureUnit = 0;
  m_lastAvailableUnits.clear();

  for (auto source : m_textureSources) {
    m_nextTextureUnit = source->bindTexture(m_nextTextureUnit, t_shader);
  }
}

void TextureRegistry::pushUsageBlock() {
  m_lastAvailableUnits.push_back(m_nextTextureUnit);
}

void TextureRegistry::popUsageBlock() {
  if (m_lastAvailableUnits.empty()) {
    throw TextureRegistryException("ERROR::TEXTURE_REGISTRY::POP");
  }
  m_nextTextureUnit = m_lastAvailableUnits.back();
  m_lastAvailableUnits.pop_back();
}
