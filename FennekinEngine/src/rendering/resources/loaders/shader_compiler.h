#pragma once

#include "core/debug/exceptions.h"
#include "rendering/resources/shader_defs.h"

#include <string>
#include <vector>

class ShaderCompilerException : public QuarkException {
  using QuarkException::QuarkException;
};

class ShaderCompiler {
 public:
  // Loads and compiles a shader, returning the shader ID. Note that the shader
  // is deleted after linking.
  unsigned int loadAndCompileShader(const ShaderSource& shaderSource,
                                    const ShaderType type);

  // Links all compiled shaders into a shader program, and deletes the shaders.
  unsigned int linkShaderProgram();

 private:
  unsigned int compileShader(const char* shaderSource, const ShaderType type);

  std::vector<unsigned int> m_shaders;
};
