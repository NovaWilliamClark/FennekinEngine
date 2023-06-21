#include "shadows.hpp"


ShadowCamera::ShadowCamera(std::shared_ptr<DirectionalLight> light,

                           float cuboidExtents, float near, float far,
                           float shadowCameraDistanceFromOrigin,
                           glm::vec3 worldUp)
    : light_(light),
      m_cuboidExtents(cuboidExtents),
      m_near(near),
      m_far(far),
      m_shadowCameraDistanceFromOrigin(shadowCameraDistanceFromOrigin),
      m_worldUp(worldUp) {}

glm::mat4 ShadowCamera::getViewTransform() const {
  return glm::lookAt(m_shadowCameraDistanceFromOrigin * -light_->getDirection(),
                     glm::vec3(0.0f), m_worldUp);
}

glm::mat4 ShadowCamera::getProjectionTransform() const {
  // Directional lights cast orthographic shadows.
  return glm::ortho(-m_cuboidExtents, m_cuboidExtents, -m_cuboidExtents,
                    m_cuboidExtents, m_near, m_far);
}

void ShadowCamera::updateUniforms(Shader& shader) {
  shader.setMat4("lightViewProjection",
                 getProjectionTransform() * getViewTransform());
}

ShadowMap::ShadowMap(int width, int height) : Framebuffer(width, height) {
  // Attach the depth texture used for the shadow map.
  // TODO: Support omnidirectional shadow maps.
  m_depthAttachment = attachTexture(
      EBufferType::DEPTH, {
                             .filtering = ETextureFiltering::NEAREST,
                             .wrapMode = TextureWrapMode::CLAMP_TO_BORDER,
                             .borderColor = glm::vec4(1.0f),
                         });
}

unsigned int ShadowMap::bindTexture(unsigned int nextTextureUnit,
                                    Shader& shader) {
  m_depthAttachment.asTexture().bindToUnit(nextTextureUnit);
  // TODO: Make this more generic.
  shader.setInt("shadowMap", nextTextureUnit);
  return nextTextureUnit + 1;
}
