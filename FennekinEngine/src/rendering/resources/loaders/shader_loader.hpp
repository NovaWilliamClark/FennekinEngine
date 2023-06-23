#pragma once
#include "rendering/resources/shader_defs.hpp"

#include <deque>
#include <string>
#include <unordered_map>
#include <unordered_set>

class ShaderLoader {
 public:
  ShaderLoader(const ShaderSource* shaderSource, const EShaderType type);
  std::string load();

 private:
  void checkShaderType(std::string const& t_shaderPath) const;
  bool alreadyLoadedOnce(std::string const& t_shaderPath);
  void cacheShaderCode(std::string const& t_shaderPath,
                       std::string const& t_shaderCode);
  std::string load(std::string const& t_shaderPath);
  std::string lookupOrLoad(std::string const& t_shaderPath);
  std::string preprocessShader(std::string const& t_shaderPath,
                               std::string const& t_shaderCode);
  std::string getIncludesTraceback() const;
  bool checkCircularInclude(std::string const& t_resolvedPath) const;

  const ShaderSource* m_shaderSource;
  const EShaderType m_shaderType;
  std::deque<std::string> m_includeChain;
  std::unordered_map<std::string, std::string> m_codeCache;
  std::unordered_set<std::string> m_onceCache;
};
