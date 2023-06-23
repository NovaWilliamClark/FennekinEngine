#pragma once

#include <assimp/scene.h>
#include "core/debug/exceptions.hpp"
#include "rendering/resources/texture.hpp"

#include <string>
#include <vector>


// The type of map, i.e. how the underlying texture is meant to be used.
enum class ETextureMapType {
  DIFFUSE = 0,
  SPECULAR,
  ROUGHNESS,
  METALLIC,
  AO,
  EMISSION,
  NORMAL,
  CUBEMAP,
  // Adding a new texture type?
  // Update allTextureTypes() below.
};

inline std::vector<ETextureMapType> allTextureTypes() {
  std::vector<ETextureMapType> textureTypes;
  for (int i = 0; i <= static_cast<int>(ETextureMapType::CUBEMAP); i++) {
    textureTypes.push_back(static_cast<ETextureMapType>(i));
  }
  return textureTypes;
}

inline std::vector<aiTextureType> textureMapTypeToAiTextureTypes(
    ETextureMapType type) {
  switch (type) {
    case ETextureMapType::DIFFUSE:
      return {aiTextureType_DIFFUSE};
    case ETextureMapType::SPECULAR:
      // Use metalness for specularity as well. When this is loaded as a
      // combined "metalnessRoughnessTexture", shaders should read the blue
      // channel.
      return {aiTextureType_SPECULAR, aiTextureType_METALNESS};
    case ETextureMapType::ROUGHNESS:
      return {aiTextureType_DIFFUSE_ROUGHNESS};
    case ETextureMapType::METALLIC:
      return {aiTextureType_METALNESS};
    case ETextureMapType::AO:
      // For whatever reason, assimp stores AO maps as "lightmaps", even though
      // there's aiTextureType_AMBIENT_OCCLUSION...
      return {aiTextureType_LIGHTMAP};
    case ETextureMapType::EMISSION:
      return {aiTextureType_EMISSIVE};
    case ETextureMapType::NORMAL:
      return {aiTextureType_NORMALS};
    default:
      LOG_CRITICAL(
          "ERROR::TEXTURE_MAP::INVALID_TEXTURE_MAP_TYPE\n" +
          std::to_string(static_cast<int>(type)));
  }
}

// A thin wrapper around a texture, with special properties.
class TextureMap {
 public:
  TextureMap(Texture texture, ETextureMapType type, bool isPacked = false)
      : m_texture(std::move(texture)), m_type(type), m_packed(isPacked) {}

  Texture& getTexture() { return m_texture; }
  [[nodiscard]] ETextureMapType getType() const { return m_type; }
  [[nodiscard]] bool isPacked() const { return m_packed; }
  void setPacked(bool packed) { m_packed = packed; }

 private:
  Texture m_texture;
  ETextureMapType m_type;
  // Whether the texture type is part of a packed texture.
  bool m_packed;
};
