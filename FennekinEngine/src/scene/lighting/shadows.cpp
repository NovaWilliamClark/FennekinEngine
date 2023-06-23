#include "shadows.hpp"


ShadowCamera::ShadowCamera(const std::shared_ptr<DirectionalLight>& t_light, const float t_cuboidExtents, float near, float far, const float t_shadowCameraDistanceFromOrigin, const glm::vec3 t_worldUp)
    : m_light(t_light),
      m_cuboidExtents(t_cuboidExtents),
      m_near(near),
      m_far(far),
      m_shadowCameraDistanceFromOrigin(t_shadowCameraDistanceFromOrigin),
      m_worldUp(t_worldUp) {}

glm::mat4 ShadowCamera::getViewTransform() const {
  return glm::lookAt(m_shadowCameraDistanceFromOrigin * -m_light->getDirection(),
                     glm::vec3(0.0f), m_worldUp);
}

glm::mat4 ShadowCamera::getProjectionTransform() const {
  // Directional lights cast orthographic shadows.
  return glm::ortho(-m_cuboidExtents, m_cuboidExtents, -m_cuboidExtents,
                    m_cuboidExtents, m_near, m_far);
}

void ShadowCamera::updateUniforms(Shader& t_shader) {
  t_shader.setMat4("lightViewProjection",
                 getProjectionTransform() * getViewTransform());
}

ShadowMap::ShadowMap(const int t_width, const int t_height) : Framebuffer(t_width, t_height) {
  // Attach the depth texture used for the shadow map.
  // TODO: Support omnidirectional shadow maps.
  m_depthAttachment = attachTexture(
      EBufferType::DEPTH, {
                             .filtering = ETextureFiltering::NEAREST,
                             .wrapMode = ETextureWrapMode::CLAMP_TO_BORDER,
                             .borderColor = glm::vec4(1.0f),
                         });
}

unsigned int ShadowMap::bindTexture(const unsigned int t_nextTextureUnit,
                                    Shader& t_shader) {
  m_depthAttachment.asTexture().bindToUnit(t_nextTextureUnit);
  // TODO: Make this more generic.
  t_shader.setInt("shadowMap", t_nextTextureUnit);
  return t_nextTextureUnit + 1;
}
