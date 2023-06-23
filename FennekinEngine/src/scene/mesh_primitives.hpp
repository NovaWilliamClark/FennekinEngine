#pragma once

#include "core/debug/exceptions.hpp"
#include "rendering/core/framebuffer.hpp"
#include "rendering/resources/texture.hpp"
#include "rendering/resources/texture_map.hpp"

#include "mesh.hpp"


class PrimitiveMesh : public Mesh {};

class PlaneMesh final : public PrimitiveMesh {
 public:
  explicit PlaneMesh(std::string t_texturePath = "");
  explicit PlaneMesh(const std::vector<TextureMap>& t_textureMaps);

 protected:
  void loadMeshAndTextures(const std::vector<TextureMap>& t_textureMaps);
  void initializeVertexAttributes() override;
};

// A unit cube.
class CubeMesh final : public PrimitiveMesh {
 public:
  explicit CubeMesh(const std::string& t_texturePath = "");
  explicit CubeMesh(const std::vector<TextureMap>& t_textureMaps);

 protected:
  void loadMeshAndTextures(const std::vector<TextureMap>& t_textureMaps);
  void initializeVertexAttributes() override;
};

// Like CubeMesh, but with normals pointing inward.
class RoomMesh final : public PrimitiveMesh {
 public:
  explicit RoomMesh(std::string t_texturePath = "");
  explicit RoomMesh(const std::vector<TextureMap>& t_textureMaps);

 protected:
  void loadMeshAndTextures(const std::vector<TextureMap>& t_textureMaps);
  void initializeVertexAttributes() override;
};

// A unit sphere, with the given number of meridians / parallels.
class SphereMesh final : public PrimitiveMesh {
 public:
  static constexpr int DEFAULT_NUM_MERIDIANS = 64;
  static constexpr int DEFAULT_NUM_PARALLELS = 64;

  explicit SphereMesh(std::string t_texturePath = "",
                      int t_numMeridians = DEFAULT_NUM_MERIDIANS,
                      int t_numParallels = DEFAULT_NUM_PARALLELS);
  explicit SphereMesh(const std::vector<TextureMap>& t_textureMaps,
                      int t_numMeridians = DEFAULT_NUM_MERIDIANS,
                      int t_numParallels = DEFAULT_NUM_PARALLELS);

 protected:
  void loadMeshAndTextures(const std::vector<TextureMap>& t_textureMaps);
  void initializeVertexAttributes() override;

  int numMeridians;
  int numParallels;
};

class SkyboxMesh final : public PrimitiveMesh {
 public:
  // Creates an unbound skybox mesh.
  SkyboxMesh();
  // Creates a new skybox mesh from a set of 6 textures for the faces. Textures
  // must be passed in order starting with GL_TEXTURE_CUBE_MAP_POSITIVE_X and
  // incrementing from there; namely, in the order right, left, top, bottom,
  // front, and back.
  explicit SkyboxMesh(std::vector<std::string> t_faces);
  explicit SkyboxMesh(Texture t_texture);

  // Sets a framebuffer attachment as the texture.
  void setTexture(Attachment t_attachment);
  // Sets the texture. This overrides previously set textures.
  void setTexture(Texture t_texture);

 protected:
  void loadMesh();
  void initializeVertexAttributes() override;
};

class ScreenQuadMesh final : public PrimitiveMesh {
 public:
  // Creates an unbound screen quad mesh.
  ScreenQuadMesh();
  // Creates a new screen quad mesh from a texture.
  explicit ScreenQuadMesh(Texture t_texture);

  // Sets a framebuffer attachment as the texture.
  void setTexture(Attachment t_attachment);
  // Sets the texture. This overrides previously set textures.
  void setTexture(Texture t_texture);
  // Unsets the texture.
  void unsetTexture();

 protected:
  void loadMesh();
  void initializeVertexAttributes() override;
  void bindTextures(Shader& t_shader, TextureRegistry* t_textureRegistry) override;
};
