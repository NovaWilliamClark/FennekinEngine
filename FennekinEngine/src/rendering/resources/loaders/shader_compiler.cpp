#include "shader_compiler.hpp"
#include "shader_loader.hpp"

#include "core/debug/logger.hpp"


inline const char* shaderTypeToString(const EShaderType t_type) {
  switch (t_type) {
    case EShaderType::VERTEX:
      return "VERTEX";
    case EShaderType::FRAGMENT:
      return "FRAGMENT";
    case EShaderType::GEOMETRY:
      return "GEOMETRY";
    case EShaderType::COMPUTE:
      return "COMPUTE";
  }
    LOG_CRITICAL("Invalid shader type");
}

inline GLenum shaderTypeToGlShaderType(const EShaderType t_type) {
  switch (t_type) {
    case EShaderType::VERTEX:
      return GL_VERTEX_SHADER;
    case EShaderType::FRAGMENT:
      return GL_FRAGMENT_SHADER;
    case EShaderType::GEOMETRY:
      return GL_GEOMETRY_SHADER;
    case EShaderType::COMPUTE:
      return GL_COMPUTE_SHADER;
  }
  LOG_CRITICAL("Invalid shader type");
}

unsigned int ShaderCompiler::loadAndCompileShader(
    const ShaderSource& t_shaderSource, const EShaderType t_type) {
  ShaderLoader shaderLoader(&t_shaderSource, t_type);
  const std::string shaderString = shaderLoader.load();
  const char* resolvedSource = shaderString.c_str();
  const unsigned int shaderId = compileShader(resolvedSource, t_type);
  m_shaders.push_back(shaderId);
  return shaderId;
}

unsigned int ShaderCompiler::linkShaderProgram() {
  int success;

  // Create and link shader program.
  const unsigned int shaderProgram = glCreateProgram();

  // Add the compiled shaders into the program.
  for (const unsigned int shaderId : m_shaders) {
    glAttachShader(shaderProgram, shaderId);
  }
  glLinkProgram(shaderProgram);

  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
    LOG_CRITICAL("Shader Compilation Failed");
  }

  // Delete shaders now that they're linked.
  for (const unsigned int shaderId : m_shaders) {
    glDeleteShader(shaderId);
  }
  m_shaders.clear();
  return shaderProgram;
}

unsigned int ShaderCompiler::compileShader(const char* t_shaderSource,
                                           const EShaderType t_type) {
  int success;

  // Compile shader.
  const GLenum glShaderType = shaderTypeToGlShaderType(t_type);
  const unsigned int shader = glCreateShader(glShaderType);

  glShaderSource(shader, 1, &t_shaderSource, nullptr);
  glCompileShader(shader);

  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetShaderInfoLog(shader, 512, nullptr, infoLog);
    std::string typeString(shaderTypeToString(t_type));
    LOG_CRITICAL("Shader Compilation Failed {} {} {}", infoLog, '\n', t_shaderSource);
  }
  return shader;
}