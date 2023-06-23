#pragma once

#include "core/debug/exceptions.hpp"
#include "rendering/resources/shader_defs.hpp"

#include <string>
#include <vector>


class ShaderCompiler {
 public:
  // Loads and compiles a shader, returning the shader ID. Note that the shader
  // is deleted after linking.
  unsigned int loadAndCompileShader(const ShaderSource& t_shaderSource,
                                    const EShaderType t_type);

  // Links all compiled shaders into a shader program, and deletes the shaders.
  unsigned int linkShaderProgram();

 private:
  static unsigned int compileShader(const char* t_shaderSource, const EShaderType t_type);

  std::vector<unsigned int> m_shaders;
};
