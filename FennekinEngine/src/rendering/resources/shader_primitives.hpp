#pragma once

#include "shader.hpp"

#include <glm/glm.hpp>

// A builtin shader for skyboxes. Should be used at the end of a frame to render
// the skybox behind all other geometry.
// TODO: Move this to a skybox header.
class SkyboxShader : public Shader {
 public:
  SkyboxShader();

  virtual void activate() override;
  virtual void deactivate() override;

  virtual void setMat4(const char* name, const glm::mat4& matrix) override;
};

class ScreenShader : public Shader {
 public:
  ScreenShader();
  explicit ScreenShader(const ShaderSource& fragmentSource);
};

class ScreenLodShader : public ScreenShader {
 public:
  ScreenLodShader();

  void setMipLevel(int mipLevel) {
    setFloat("lod", static_cast<float>(mipLevel));
  }
};

// TODO: Move this to the shadows header.
class ShadowMapShader : public Shader {
 public:
  ShadowMapShader();
};