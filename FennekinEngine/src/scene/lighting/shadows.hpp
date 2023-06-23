#pragma once

#include "core/debug/exceptions.hpp"
#include "rendering/core/framebuffer.hpp"
#include "scene/lighting/light.hpp"
#include "rendering/resources/shader.hpp"
#include "rendering/resources/texture.hpp"
#include "rendering/registers/texture_registry.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class ShadowCamera final : public UniformSource {
 public:
  // TODO: Currently only renders the origin. Make this more dynamic, and have
  // it automatically determine a best-fit frustum based on the scene.
  explicit ShadowCamera(const std::shared_ptr<DirectionalLight>& t_light,
                        float t_cuboidExtents = 10.0f, float near = 0.1f,
                        float far = 15.0f, float t_shadowCameraDistanceFromOrigin = 7.0f,
                        glm::vec3 t_worldUp = glm::vec3(0.0f, 1.0f, 0.0f));
  ~ShadowCamera() override = default;

  float getCuboidExtents() const { return m_cuboidExtents; }
  void setCuboidExtents(const float t_cuboidExtents) { m_cuboidExtents = t_cuboidExtents; }
  float getNearPlane() const { return m_near; }
  void setNearPlane(const float t_near) { m_near = t_near; }
  float getFarPlane() const { return m_far; }
  void setFarPlane(const float t_far) { m_far = t_far; }
  float getDistanceFromOrigin() const {
    return m_shadowCameraDistanceFromOrigin;
  }
  void setDistanceFromOrigin(const float t_dist) {
    m_shadowCameraDistanceFromOrigin = t_dist;
  }

  glm::mat4 getViewTransform() const;
  glm::mat4 getProjectionTransform() const;

  void updateUniforms(Shader& t_shader) override;

 private:
  std::shared_ptr<DirectionalLight> m_light;
  // The extents of the ortho camera's rendering cuboid.
  float m_cuboidExtents;
  // The nearPlane plane of the ortho cuboid.
  float m_near;
  // The farPlane plane of the ortho cuboid.
  float m_far;
  // The fake distance from the origin that the shadow camera is positioned at.
  float m_shadowCameraDistanceFromOrigin;
  glm::vec3 m_worldUp;
};

class ShadowMap final : public Framebuffer, public TextureSource {
 public:
  explicit ShadowMap(int t_width = 1024, int t_height = 1024);
  explicit ShadowMap(const ImageSize t_size) : ShadowMap(t_size.width, t_size.height) {}
  ~ShadowMap() override = default;

  Texture getDepthTexture() { return m_depthAttachment.asTexture(); }
  unsigned int bindTexture(unsigned int t_nextTextureUnit,
                           Shader& t_shader) override;

 private:
  Attachment m_depthAttachment;
};
