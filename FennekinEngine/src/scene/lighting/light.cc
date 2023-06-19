#include "light.h"

void LightRegistry::addLight(Light* light) {
  // TODO: Throw an error if this exceeds the max light count supported in the
  // shader.
  m_lights.push_back(light);

  switch (light->getLightType()) {
    case ELightType::DIRECTIONAL_LIGHT:
      light->setLightIdx(m_directionalCount);
      m_directionalCount++;
      break;
    case ELightType::POINT_LIGHT:
      light->setLightIdx(m_pointCount);
      m_pointCount++;
      break;
    case ELightType::SPOT_LIGHT:
      light->setLightIdx(m_spotCount);
      m_spotCount++;
      break;
  }
}

void LightRegistry::updateUniforms(Shader& shader) {
  if (viewSource_ != nullptr) {
    applyViewTransform(viewSource_->getViewTransform());
  }
  shader.setInt("fnk_directionalLightCount", m_directionalCount);
  shader.setInt("fnk_pointLightCount", m_pointCount);
  shader.setInt("fnk_spotLightCount", m_spotCount);

  for (auto light : m_lights) {
    light->updateUniforms(shader);
  }
}

void LightRegistry::applyViewTransform(const glm::mat4& view) {
  // TODO: Only do this when we need to.
  for (auto light : m_lights) {
    light->applyViewTransform(view);
  }
}

void LightRegistry::setUseViewTransform(bool useViewTransform) {
  for (auto light : m_lights) {
    light->setUseViewTransform(useViewTransform);
  }
}

DirectionalLight::DirectionalLight(glm::vec3 direction, glm::vec3 diffuse,
                                   glm::vec3 specular)
    : m_direction(glm::normalize(direction)),
      m_diffuse(diffuse),
      m_specular(specular) {}

void DirectionalLight::updateUniforms(Shader& shader) {
  checkState();

  if (hasViewBeenApplied) {
    shader.setVec3(uniformName + ".direction",
                   useViewTransform ? m_viewDirection : m_direction);
  }
  if (hasLightChanged) {
    shader.setVec3(uniformName + ".diffuse", m_diffuse);
    shader.setVec3(uniformName + ".specular", m_specular);
  }

  // TODO: Fix change detection to work with >1 shaders.
  // resetChangeDetection();
}

void DirectionalLight::applyViewTransform(const glm::mat4& view) {
  m_viewDirection = glm::vec3(view * glm::vec4(m_direction, 0.0f));
  hasViewBeenApplied = true;
}

PointLight::PointLight(glm::vec3 position, glm::vec3 diffuse,
                       glm::vec3 specular, Attenuation attenuation)
    : m_position(position),
      m_diffuse(diffuse),
      m_specular(specular),
      m_attenuation(attenuation) {}

void PointLight::updateUniforms(Shader& shader) {
  checkState();

  if (hasViewBeenApplied) {
    shader.setVec3(uniformName + ".position",
                   useViewTransform ? m_viewPosition : m_position);
  }
  if (hasLightChanged) {
    shader.setVec3(uniformName + ".diffuse", m_diffuse);
    shader.setVec3(uniformName + ".specular", m_specular);
    shader.setFloat(uniformName + ".attenuation.constant",
                    m_attenuation.constant);
    shader.setFloat(uniformName + ".attenuation.linear", m_attenuation.linear);
    shader.setFloat(uniformName + ".attenuation.quadratic",
                    m_attenuation.quadratic);
  }

  // resetChangeDetection();
}

void PointLight::applyViewTransform(const glm::mat4& view) {
  m_viewPosition = glm::vec3(view * glm::vec4(m_position, 1.0f));
  hasViewBeenApplied = true;
}

SpotLight::SpotLight(glm::vec3 position, glm::vec3 direction, float innerAngle,
                     float outerAngle, glm::vec3 diffuse, glm::vec3 specular,
                     Attenuation attenuation)
    : m_position(position),
      m_direction(direction),
      m_innerAngle(innerAngle),
      m_outerAngle(outerAngle),
      m_diffuse(diffuse),
      m_specular(specular),
      m_attenuation(attenuation) {}

void SpotLight::updateUniforms(Shader& shader) {
  checkState();

  if (hasViewBeenApplied) {
    shader.setVec3(uniformName + ".position",
                   useViewTransform ? m_viewPosition : m_position);
    shader.setVec3(uniformName + ".direction",
                   useViewTransform ? m_viewDirection : m_direction);
  }
  if (hasLightChanged) {
    shader.setFloat(uniformName + ".innerAngle", m_innerAngle);
    shader.setFloat(uniformName + ".outerAngle", m_outerAngle);
    shader.setVec3(uniformName + ".diffuse", m_diffuse);
    shader.setVec3(uniformName + ".specular", m_specular);
    shader.setFloat(uniformName + ".attenuation.constant",
                    m_attenuation.constant);
    shader.setFloat(uniformName + ".attenuation.linear", m_attenuation.linear);
    shader.setFloat(uniformName + ".attenuation.quadratic",
                    m_attenuation.quadratic);
  }

  // resetChangeDetection();
}

void SpotLight::applyViewTransform(const glm::mat4& view) {
  m_viewPosition = glm::vec3(view * glm::vec4(m_position, 1.0f));
  m_viewDirection = glm::vec3(view * glm::vec4(m_direction, 0.0f));
  hasViewBeenApplied = true;
}
