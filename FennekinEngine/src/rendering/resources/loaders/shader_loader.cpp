#include "shader_loader.hpp"

#include "core/debug/logger.hpp"
#include "rendering/resources/shader_defs.hpp"
#include "utilities/utils.hpp"

#include <fstream>
#include <regex>
#include <sstream>

std::string readFile(std::string const& t_path) {
  std::ifstream file;
  file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  file.open(t_path.c_str());
  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

void ShaderLoader::checkShaderType(std::string const& t_shaderPath) const {
  // Allow ".glsl" as a generic shader suffix (e.g. for type-agnostic shader
  // code).
  if (stringHasSuffix(t_shaderPath, ".glsl")) return;

  switch (m_shaderType) {
    case EShaderType::VERTEX:
      if (!stringHasSuffix(t_shaderPath, ".vert")) {
        LOG_CRITICAL(
            "ERROR::SHADER_LOADER::INVALID_EXTENSION\n"
            "Loaded vertex shader '" +
            t_shaderPath + "' must end in '.vert'.");
      }
      break;
    case EShaderType::FRAGMENT:
      if (!stringHasSuffix(t_shaderPath, ".frag")) {
        LOG_CRITICAL(
            "ERROR::SHADER_LOADER::INVALID_EXTENSION\n"
            "Loaded fragment shader '" +
            t_shaderPath + "' must end in '.frag'.");
      }
      break;
    case EShaderType::GEOMETRY:
      if (!stringHasSuffix(t_shaderPath, ".geom")) {
        LOG_CRITICAL(
            "ERROR::SHADER_LOADER::INVALID_EXTENSION\n"
            "Loaded geometry shader '" +
            t_shaderPath + "' must end in '.geom'.");
      }
      break;
    case EShaderType::COMPUTE:
      if (!stringHasSuffix(t_shaderPath, ".comp")) {
        LOG_CRITICAL(
            "ERROR::SHADER_LOADER::INVALID_EXTENSION\n"
            "Loaded compute shader '" +
            t_shaderPath + "' must end in '.comp'.");
      }
      break;
  }
}

bool ShaderLoader::alreadyLoadedOnce(std::string const& t_shaderPath) {
  const std::string resolvedPath = resolvePath(t_shaderPath);
  const auto item = m_onceCache.find(resolvedPath);
  return item != m_onceCache.end();
}

std::string ShaderLoader::getIncludesTraceback() const {
  std::stringstream buffer;
  for (const std::string& path : m_includeChain) {
    buffer << "  > " << path << std::endl;
  }
  return buffer.str();
}

bool ShaderLoader::checkCircularInclude(std::string const& t_resolvedPath) const {
  for (const std::string& path : m_includeChain) {
    if (path == t_resolvedPath) {
      return true;
    }
  }
  return false;
}

ShaderLoader::ShaderLoader(const ShaderSource* shaderSource,
                           const EShaderType type)
    : m_shaderSource(shaderSource), m_shaderType(type) {}

std::string ShaderLoader::lookupOrLoad(std::string const& t_shaderPath) {
  const std::string resolvedPath = resolvePath(t_shaderPath);

  const auto item = m_codeCache.find(resolvedPath);
  if (item != m_codeCache.end()) {
    // Cache hit; return code.
    return item->second;
  }

  // Cache miss; read code from file.
  std::string shaderCode;
  try {
    shaderCode = readFile(t_shaderPath);
  } catch (std::ifstream::failure& exception) {
    const std::string traceback = getIncludesTraceback();
    LOG_CRITICAL(
        "ERROR::SHADER_LOADER::FILE_NOT_SUCCESSFULLY_READ\n"
        "Unable to read shader '" +
        std::string(t_shaderPath) + "', traceback below (most recent last):\n" +
        traceback);
  }
  // Don't cache the code just yet, because we need to preprocess it.
  return shaderCode;
}

std::string ShaderLoader::load(std::string const& t_shaderPath) {
  checkShaderType(t_shaderPath);

  // Handle #pragma once.
  if (alreadyLoadedOnce(t_shaderPath)) {
    return "";
  }

  const auto shaderCode = lookupOrLoad(t_shaderPath);
  auto processedCode = preprocessShader(t_shaderPath, shaderCode);
  cacheShaderCode(t_shaderPath, processedCode);
  return processedCode;
}

void ShaderLoader::cacheShaderCode(std::string const& t_shaderPath,
                                   std::string const& t_shaderCode) {
  const std::string resolvedPath = resolvePath(t_shaderPath);
  m_codeCache[resolvedPath] = t_shaderCode;
}

std::string ShaderLoader::preprocessShader(std::string const& t_shaderPath,
                                           std::string const& t_shaderCode) {
  const std::string resolvedPath = resolvePath(t_shaderPath);
  m_includeChain.push_back(resolvedPath);

  const std::regex oncePattern(R"(((^|\r?\n)\s*)#pragma\s+once\s*(?=\r?\n|$))");
  if (std::regex_search(t_shaderCode, oncePattern)) {
    m_onceCache.insert(resolvedPath);
  }

  const std::regex includePattern(
      R"(((^|\r?\n)\s*)#pragma\s+fnk_include\s+(".*"|<.*>)(?=\r?\n|$))");
  std::string processedCode = regexReplace(
      t_shaderCode, includePattern, [this, t_shaderPath](const std::smatch& t_m) {
          const std::string whitespace = t_m[1];
        // Extract the path.
        const std::string incl = t_m[3];
        const char inclType = incl[0];
        const std::string path = trim(incl.substr(1, incl.size() - 2));

        if (inclType == '<') {
          // fnk include.
          return whitespace + load("content/shaders/" + path);
        } else {
          // Standard include.
          const size_t i = t_shaderPath.find_last_of('/');
          // This will either be the current shader's directory, or empty
          // string if the current shader is at project root.
          const std::string prefix =
              i != std::string::npos ? t_shaderPath.substr(0, i + 1) : "";
          return whitespace + load(prefix + path);
        }
      });

  m_includeChain.pop_back();
  return processedCode;
}

std::string ShaderLoader::load() {
  // Handle either loading from file, or loading from inline source.
  m_onceCache.clear();
  if (m_shaderSource->isPath()) {
    return load(m_shaderSource->value);
  } else {
    return preprocessShader(".", m_shaderSource->value);
  }
}
