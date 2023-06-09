#pragma once

#include <gl/glew.h>
#include "core/debug/exceptions.hpp"
#include "rendering/resources/shader_defs.hpp"
#include "rendering/resources/texture.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <string>
#include <vector>

class Shader;


// An interface for a unified way of configuring shader uniforms.
// TODO: Extract to a "UniformContext" class that is used by the shader, rather
// than the shader collecting UniformSources directly.
class UniformSource {
 public:
    virtual ~UniformSource() = default;
    virtual void updateUniforms(Shader& shader) = 0;
};

class Shader {
 public:
  Shader(const ShaderSource& vertexSource, const ShaderSource& fragmentSource);
  Shader(const ShaderSource& vertexSource, const ShaderSource& fragmentSource,
         const ShaderSource& geometrySource);
  virtual ~Shader() = default;

  unsigned int getProgramId() const { return shaderProgram; }

  virtual void activate();
  virtual void deactivate();

  void addUniformSource(std::shared_ptr<UniformSource> source);
  void updateUniforms();

  // Functions for uniforms.

  virtual void setBool(const char* name, bool value);
  void setBool(std::string name, bool value) { setBool(name.c_str(), value); }
  virtual void setUInt(const char* name, unsigned int value);
  void setUInt(std::string name, unsigned int value) {
    setUInt(name.c_str(), value);
  }
  virtual void setInt(const char* name, int value);
  void setInt(std::string name, int value) { setInt(name.c_str(), value); }
  virtual void setFloat(const char* name, float value);
  void setFloat(std::string name, float value) {
    setFloat(name.c_str(), value);
  }
  virtual void setVec3(const char* name, const glm::vec3& vector);
  void setVec3(std::string name, const glm::vec3& vector) {
    setVec3(name.c_str(), vector);
  }
  virtual void setVec3(const char* name, float v0, float v1, float v2);
  void setVec3(std::string name, float v0, float v1, float v2) {
    setVec3(name.c_str(), v0, v1, v2);
  }
  virtual void setMat4(const char* name, const glm::mat4& matrix);
  void setMat4(std::string name, const glm::mat4& matrix) {
    setMat4(name.c_str(), matrix);
  }

 protected:
  Shader();
  int safeGetUniformLocation(const char* name) const;

  unsigned int shaderProgram;
  std::vector<std::shared_ptr<UniformSource>> uniformSources;
};

class ComputeShader : public Shader {
 public:
  explicit ComputeShader(const ShaderSource& computeSource);
  virtual ~ComputeShader() = default;

  // Dispatches a compute shader execution and writes to the given image
  // texture. Assumes that the texture has already been bound to the correct
  // texture unit.
  void dispatchToTexture(Texture& texture);
};
