#pragma once

#include "core/debug/exceptions.hpp"
#include "rendering/core/framebuffer.hpp"
#include "scene/lighting/light.hpp"
#include "rendering/resources/shader.hpp"
#include "rendering/resources/texture.hpp"
#include "rendering/registers/texture_registry.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class ShadowException : public QuarkException {
  using QuarkException::QuarkException;
};

class ShadowCamera : public UniformSource {
 public:
  // TODO: Currently only renders the origin. Make this more dynamic, and have
  // it automatically determine a best-fit frustum based on the scene.
  ShadowCamera(std::shared_ptr<DirectionalLight> light,
               float cuboidExtents = 10.0f, float near = 0.1f,
               float far = 15.0f, float shadowCameraDistanceFromOrigin = 7.0f,
               glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f));
  virtual ~ShadowCamera() = default;

  float getCuboidExtents() const { return m_cuboidExtents; }
  void setCuboidExtents(float cuboidExtents) { m_cuboidExtents = cuboidExtents; }
  float getNearPlane() const { return m_near; }
  void setNearPlane(float t_near) { m_near = t_near; }
  float getFarPlane() const { return m_far; }
  void setFarPlane(float t_far) { m_far = t_far; }
  float getDistanceFromOrigin() const {
    return m_shadowCameraDistanceFromOrigin;
  }
  void setDistanceFromOrigin(float dist) {
    m_shadowCameraDistanceFromOrigin = dist;
  }

  glm::mat4 getViewTransform() const;
  glm::mat4 getProjectionTransform() const;

  void updateUniforms(Shader& shader) override;

 private:
  std::shared_ptr<DirectionalLight> light_;
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

class ShadowMap : public Framebuffer, public TextureSource {
 public:
  explicit ShadowMap(int width = 1024, int height = 1024);
  explicit ShadowMap(ImageSize size) : ShadowMap(size.width, size.height) {}
  virtual ~ShadowMap() = default;

  Texture getDepthTexture() { return m_depthAttachment.asTexture(); }
  unsigned int bindTexture(unsigned int nextTextureUnit,
                           Shader& shader) override;

 private:
  Attachment m_depthAttachment;
};
