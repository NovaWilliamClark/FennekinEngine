#include "core/core.hpp"
#include "shader.hpp"
#include "rendering/resources/loaders/shader_compiler.hpp"
#include "rendering/resources/loaders/shader_loader.hpp"

Shader::Shader(const ShaderSource& vertexSource,
               const ShaderSource& fragmentSource) {
  ShaderCompiler compiler;
  compiler.loadAndCompileShader(vertexSource, EShaderType::VERTEX);
  compiler.loadAndCompileShader(fragmentSource, EShaderType::FRAGMENT);
  shaderProgram = compiler.linkShaderProgram();
}

Shader::Shader(const ShaderSource& vertexSource,
               const ShaderSource& fragmentSource,
               const ShaderSource& geometrySource) {
  ShaderCompiler compiler;
  compiler.loadAndCompileShader(vertexSource, EShaderType::VERTEX);
  compiler.loadAndCompileShader(fragmentSource, EShaderType::FRAGMENT);
  compiler.loadAndCompileShader(geometrySource, EShaderType::GEOMETRY);
  shaderProgram = compiler.linkShaderProgram();
}

Shader::Shader():
    shaderProgram(0) {
}

int Shader::safeGetUniformLocation(const char* name) const {
    const int uniform = glGetUniformLocation(shaderProgram, name);
  if (uniform == -1) {
      // Do Nothing
      // If needed replace with a Log message
  }
  return uniform;
}

void Shader::activate() { glUseProgram(shaderProgram); }
void Shader::deactivate() { glUseProgram(0); }

// TODO: Is shared_ptr really the best approach here?
void Shader::addUniformSource(std::shared_ptr<UniformSource> source) {
  uniformSources.push_back(source);
}

void Shader::updateUniforms() {
  // Update core uniforms.
  setFloat("fnk_time", Fnk::time());

  for (const auto& uniformSource : uniformSources) {
    uniformSource->updateUniforms(*this);
  }
}

void Shader::setBool(const char* name, bool value) {
  activate();
  glUniform1i(safeGetUniformLocation(name), static_cast<int>(value));
}

void Shader::setUInt(const char* name, unsigned int value) {
  activate();
  glUniform1ui(safeGetUniformLocation(name), value);
}

void Shader::setInt(const char* name, int value) {
  activate();
  glUniform1i(safeGetUniformLocation(name), value);
}

void Shader::setFloat(const char* name, float value) {
  activate();
  glUniform1f(safeGetUniformLocation(name), value);
}

void Shader::setVec3(const char* name, const glm::vec3& vector) {
  activate();
  glUniform3fv(safeGetUniformLocation(name), /*count=*/1,
               glm::value_ptr(vector));
}

void Shader::setVec3(const char* name, float v0, float v1, float v2) {
  activate();
  glUniform3f(safeGetUniformLocation(name), v0, v1, v2);
}

void Shader::setMat4(const char* name, const glm::mat4& matrix) {
  activate();
  glUniformMatrix4fv(safeGetUniformLocation(name), /*count=*/1,
                     /*transpose=*/GL_FALSE, glm::value_ptr(matrix));
}

ComputeShader::ComputeShader(const ShaderSource& computeSource) {
  ShaderCompiler compiler;
  compiler.loadAndCompileShader(computeSource, EShaderType::COMPUTE);
  shaderProgram = compiler.linkShaderProgram();
}

void ComputeShader::dispatchToTexture(Texture& texture) {
  activate();
  texture.bindToUnit(0, ETextureBindType::IMAGE_TEXTURE);
  glDispatchCompute(texture.getWidth(), texture.getHeight(), 1);

  // Guard until writing is complete.
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}
