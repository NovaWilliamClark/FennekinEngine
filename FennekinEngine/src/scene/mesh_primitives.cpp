#include "mesh_primitives.hpp"

#include "core/debug/logger.hpp"


// clang-format off
constexpr float planeVertices[] = {
    // positions           // normals            // tangents           // texture coords
    -0.5f,  0.0f, -0.5f,   0.0f,  1.0f,  0.0f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
     0.5f,  0.0f,  0.5f,   0.0f,  1.0f,  0.0f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
     0.5f,  0.0f, -0.5f,   0.0f,  1.0f,  0.0f,   1.0f,  0.0f,  0.0f,   1.0f, 1.0f,

     0.5f,  0.0f,  0.5f,   0.0f,  1.0f,  0.0f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
    -0.5f,  0.0f, -0.5f,   0.0f,  1.0f,  0.0f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
    -0.5f,  0.0f,  0.5f,   0.0f,  1.0f,  0.0f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f
};
// clang-format on

PlaneMesh::PlaneMesh(std::string t_texturePath) {
  std::vector<TextureMap> textureMaps;
  if (!t_texturePath.empty()) {
    TextureMap textureMap(Texture::load(t_texturePath.c_str()),
                          ETextureMapType::DIFFUSE);
    textureMaps.push_back(textureMap);
  }
  loadMeshAndTextures(textureMaps);
}

PlaneMesh::PlaneMesh(const std::vector<TextureMap>& t_textureMaps) {
  loadMeshAndTextures(t_textureMaps);
}

void PlaneMesh::loadMeshAndTextures(
    const std::vector<TextureMap>& t_textureMaps) {
  constexpr unsigned int planeVertexSizeBytes = 11 * sizeof(float);
  loadMeshData(planeVertices, sizeof(planeVertices) / planeVertexSizeBytes,
               planeVertexSizeBytes, /*indices=*/{}, t_textureMaps);
}

void PlaneMesh::initializeVertexAttributes() {
  // Positions.
  vertexArray.addVertexAttrib(3, GL_FLOAT);
  // Normals.
  vertexArray.addVertexAttrib(3, GL_FLOAT);
  // Tangents.
  vertexArray.addVertexAttrib(3, GL_FLOAT);
  // Texture coordinates.
  vertexArray.addVertexAttrib(2, GL_FLOAT);

  vertexArray.finalizeVertexAttribs();
}

// clang-format off
constexpr float CUBE_VERTICES[] = {
    // positions           // normals            // tangents           // texture coords

    // back
    -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,

    // front
    -0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f,

    // left
    -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   0.0f,  0.0f,  1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.0f,  0.0f,  1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   0.0f,  0.0f,  1.0f,   1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   0.0f,  0.0f,  1.0f,   1.0f, 1.0f,

    // right
     0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   0.0f,  0.0f, -1.0f,   0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   0.0f,  0.0f, -1.0f,   1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   0.0f,  0.0f, -1.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   0.0f,  0.0f, -1.0f,   0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   0.0f,  0.0f, -1.0f,   0.0f, 0.0f,

    // bottom
    -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f,

    // top
    -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f
};
// clang-format on

CubeMesh::CubeMesh(const std::string& t_texturePath) {
  std::vector<TextureMap> textureMaps;
  if (!t_texturePath.empty()) {
      const TextureMap textureMap(Texture::load(t_texturePath.c_str()),
                                  ETextureMapType::DIFFUSE);
    textureMaps.push_back(textureMap);
  }
  loadMeshAndTextures(textureMaps);
}

CubeMesh::CubeMesh(const std::vector<TextureMap>& t_textureMaps) {
  loadMeshAndTextures(t_textureMaps);
}

void CubeMesh::loadMeshAndTextures(const std::vector<TextureMap>& t_textureMaps) {
  constexpr unsigned int cubeVertexSizeBytes = 11 * sizeof(float);
  loadMeshData(CUBE_VERTICES, sizeof(CUBE_VERTICES) / cubeVertexSizeBytes,
               cubeVertexSizeBytes, /*indices=*/{}, t_textureMaps);
}

void CubeMesh::initializeVertexAttributes() {
  // Positions.
  vertexArray.addVertexAttrib(3, GL_FLOAT);
  // Normals.
  vertexArray.addVertexAttrib(3, GL_FLOAT);
  // Tangents.
  vertexArray.addVertexAttrib(3, GL_FLOAT);
  // Texture coordinates.
  vertexArray.addVertexAttrib(2, GL_FLOAT);

  vertexArray.finalizeVertexAttribs();
}

// clang-format off
constexpr float ROOM_VERTICES[] = {
    // positions           // normals            // tangents           // texture coords

    // back
    -0.5f, -0.5f, -0.5f,   0.0f,  0.0f,  1.0f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,   0.0f,  0.0f,  1.0f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,   0.0f,  0.0f,  1.0f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,   0.0f,  0.0f,  1.0f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,   0.0f,  0.0f,  1.0f,   1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,   0.0f,  0.0f,  1.0f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,

    // front
    -0.5f, -0.5f,  0.5f,   0.0f,  0.0f, -1.0f,  -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,   0.0f,  0.0f, -1.0f,  -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,   0.0f,  0.0f, -1.0f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,   0.0f,  0.0f, -1.0f,  -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,   0.0f,  0.0f, -1.0f,  -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,   0.0f,  0.0f, -1.0f,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,

    // left
    -0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   0.0f,  0.0f, -1.0f,   0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   0.0f,  0.0f, -1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   0.0f,  0.0f, -1.0f,   0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   0.0f,  0.0f, -1.0f,   1.0f, 0.0f,

    // right
     0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   0.0f,  0.0f,  1.0f,   0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.0f,  0.0f,  1.0f,   1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.0f,  0.0f,  1.0f,   1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.0f,  0.0f,  1.0f,   1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   0.0f,  0.0f,  1.0f,   0.0f, 1.0f,

    // bottom
    -0.5f, -0.5f, -0.5f,   0.0f,  1.0f,  0.0f,  -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,   0.0f,  1.0f,  0.0f,  -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,   0.0f,  1.0f,  0.0f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,   0.0f,  1.0f,  0.0f,  -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,   0.0f,  1.0f,  0.0f,  -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,   0.0f,  1.0f,  0.0f,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,

    // top
    -0.5f,  0.5f, -0.5f,   0.0f, -1.0f,  0.0f,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,   0.0f, -1.0f,  0.0f,  -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,   0.0f, -1.0f,  0.0f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,   0.0f, -1.0f,  0.0f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,   0.0f, -1.0f,  0.0f,  -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,   0.0f, -1.0f,  0.0f,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f
};
// clang-format on

RoomMesh::RoomMesh(std::string t_texturePath) {
  std::vector<TextureMap> textureMaps;
  if (!t_texturePath.empty()) {
      const TextureMap textureMap(Texture::load(t_texturePath.c_str()),
                                  ETextureMapType::DIFFUSE);
    textureMaps.push_back(textureMap);
  }
  loadMeshAndTextures(textureMaps);
}

RoomMesh::RoomMesh(const std::vector<TextureMap>& t_textureMaps) {
  loadMeshAndTextures(t_textureMaps);
}

void RoomMesh::loadMeshAndTextures(const std::vector<TextureMap>& t_textureMaps) {
  constexpr unsigned int roomVertexSizeBytes = 11 * sizeof(float);
  loadMeshData(ROOM_VERTICES, sizeof(ROOM_VERTICES) / roomVertexSizeBytes,
               roomVertexSizeBytes, /*indices=*/{}, t_textureMaps);
}

void RoomMesh::initializeVertexAttributes() {
  // Positions.
  vertexArray.addVertexAttrib(3, GL_FLOAT);
  // Normals.
  vertexArray.addVertexAttrib(3, GL_FLOAT);
  // Tangents.
  vertexArray.addVertexAttrib(3, GL_FLOAT);
  // Texture coordinates.
  vertexArray.addVertexAttrib(2, GL_FLOAT);

  vertexArray.finalizeVertexAttribs();
}

SphereMesh::SphereMesh(std::string t_texturePath, int t_numMeridians,
                       int t_numParallels)
    : numMeridians(t_numMeridians), numParallels(t_numParallels) {
  std::vector<TextureMap> textureMaps;
  if (!t_texturePath.empty()) {
      const TextureMap textureMap(Texture::load(t_texturePath.c_str()),
                                  ETextureMapType::DIFFUSE);
    textureMaps.push_back(textureMap);
  }
  loadMeshAndTextures(textureMaps);
}

SphereMesh::SphereMesh(const std::vector<TextureMap>& t_textureMaps,
                       int t_numMeridians, int t_numParallels)
    : numMeridians(t_numMeridians), numParallels(t_numParallels) {
  loadMeshAndTextures(t_textureMaps);
}

void SphereMesh::loadMeshAndTextures(
    const std::vector<TextureMap>& t_textureMaps) {
  // Generate the sphere vertex components. This uses the common "UV" approach.

  constexpr auto PI = glm::pi<float>();

  // Always use at least 3 meridians.
  const unsigned int widthSegments = glm::max(numMeridians, 3);
  // Add two segments to account for the poles.
  const unsigned int heightSegments = glm::max(numParallels, 2);

  std::vector<float> vertexData;
  // We use <= instead of < because we want to create one more "layer" of
  // vertices in order for UVs to work properly.
  for (unsigned int iy = 0; iy <= heightSegments; ++iy) {
    // We can subdividue the surface into meridians / parallels, and treat
    // these as UVs since they lie in the range [0, 1].
    float v = iy / static_cast<float>(heightSegments);
    // However, we adjust it by a tiny amount to ensure that we don't generate
    // tangents of zero length (due to the sin() in the calculations).
    // TODO: Is this reasonable?
    v += 1e-5;
    // Use a special offset for the poles.
    float uOffset = 0.0f;
    if (iy == 0) {
      uOffset = 0.5f / widthSegments;
    } else if (iy == heightSegments) {
      uOffset = -0.5f / widthSegments;
    }
    for (unsigned int ix = 0; ix <= widthSegments; ++ix) {
      float u = ix / static_cast<float>(widthSegments);

      float x = cos(u * 2.0f * PI) * sin(v * PI);
      float y = cos(v * PI);
      float z = sin(u * 2.0f * PI) * sin(v * PI);

      // Positions.
      vertexData.push_back(x);
      vertexData.push_back(y);
      vertexData.push_back(z);
      // Normals. Unit sphere makes this easy. 8D
      vertexData.push_back(x);
      vertexData.push_back(y);
      vertexData.push_back(z);
      // Tangents. Rotated 90 degrees about the origin.
      vertexData.push_back(-z);
      vertexData.push_back(0.0f);  // Spherical tangents ignore parallels.
      vertexData.push_back(x);
      // Texture coordinates.
      vertexData.push_back(u + uOffset);
      vertexData.push_back(
          1.0f - v);  // We're generating the sphere top-down, so reverse V.
    }
  }

  std::vector<unsigned int> indices;
  // Since we created an extra duplicate "wraparound" vertex for each parallel,
  // we have to adjust the stride.
  const unsigned int widthStride = widthSegments + 1;
  for (unsigned int iy = 0; iy < heightSegments; ++iy) {
    for (unsigned int ix = 0; ix < widthSegments; ++ix) {
      // Form triangles from quad vertices.
      unsigned int a = iy * widthStride + (ix + 1);
      unsigned int b = iy * widthStride + ix;
      unsigned int c = (iy + 1) * widthStride + ix;
      unsigned int d = (iy + 1) * widthStride + (ix + 1);

      if (iy != 0) {
        indices.push_back(a);
        indices.push_back(b);
        indices.push_back(d);
      }
      if (iy != heightSegments - 1) {
        indices.push_back(b);
        indices.push_back(c);
        indices.push_back(d);
      }
    }
  }

  constexpr unsigned int sphereVertexSizeBytes = 11 * sizeof(float);
  loadMeshData(vertexData.data(),
               (sizeof(float) * vertexData.size()) / sphereVertexSizeBytes,
               sphereVertexSizeBytes, indices, t_textureMaps);
}

void SphereMesh::initializeVertexAttributes() {
  // Positions.
  vertexArray.addVertexAttrib(3, GL_FLOAT);
  // Normals.
  vertexArray.addVertexAttrib(3, GL_FLOAT);
  // Tangents.
  vertexArray.addVertexAttrib(3, GL_FLOAT);
  // Texture coordinates.
  vertexArray.addVertexAttrib(2, GL_FLOAT);

  vertexArray.finalizeVertexAttribs();
}

// clang-format off
constexpr float SKYBOX_VERTICES[] = {
    // positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};
// clang-format on

SkyboxMesh::SkyboxMesh() { loadMesh(); }

SkyboxMesh::SkyboxMesh(std::vector<std::string> t_faces)
    : SkyboxMesh(Texture::loadCubemap(t_faces)) {}

SkyboxMesh::SkyboxMesh(Texture t_texture) {
  loadMesh();
  setTexture(t_texture);
}

void SkyboxMesh::setTexture(Attachment t_attachment) {
  setTexture(t_attachment.asTexture());
}

void SkyboxMesh::setTexture(Texture t_texture) {
  if (t_texture.getType() != ETextureType::CUBEMAP) {
    LOG_CRITICAL("Mesh Primative, Invalid Texture Type");
  }
  // TODO: This copies the texture info, meaning it won't see updates.
  textureMaps.clear();
  textureMaps.emplace_back(t_texture, ETextureMapType::CUBEMAP);
}

void SkyboxMesh::loadMesh() {
  constexpr unsigned int skyboxVertexSizeBytes = 3 * sizeof(float);
  loadMeshData(SKYBOX_VERTICES, sizeof(SKYBOX_VERTICES) / skyboxVertexSizeBytes,
               skyboxVertexSizeBytes, /*indices=*/{}, /*textureMaps=*/{});
}

void SkyboxMesh::initializeVertexAttributes() {
  // Skybox only has vertex positions.
  vertexArray.addVertexAttrib(3, GL_FLOAT);
  vertexArray.finalizeVertexAttribs();
}

// clang-format off
constexpr float screenQuadVertices[] = {
    // positions   // texture coords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
};
// clang-format on

ScreenQuadMesh::ScreenQuadMesh() { loadMesh(); }

ScreenQuadMesh::ScreenQuadMesh(Texture t_texture) {
  loadMesh();
  setTexture(t_texture);
}

void ScreenQuadMesh::loadMesh() {
  constexpr unsigned int quadVertexSizeBytes = 4 * sizeof(float);
  loadMeshData(screenQuadVertices,
               sizeof(screenQuadVertices) / quadVertexSizeBytes,
               quadVertexSizeBytes, /*indices=*/{}, /*textureMaps=*/{});
}

void ScreenQuadMesh::setTexture(Attachment t_attachment) {
  setTexture(t_attachment.asTexture());
}

void ScreenQuadMesh::setTexture(Texture t_texture) {
  // TODO: This copies the texture info, meaning it won't see updates.
  textureMaps.clear();
  textureMaps.emplace_back(t_texture, ETextureMapType::DIFFUSE);
}

void ScreenQuadMesh::unsetTexture() { textureMaps.clear(); }

void ScreenQuadMesh::initializeVertexAttributes() {
  // Screen positions.
  vertexArray.addVertexAttrib(2, GL_FLOAT);
  // Texture coordinates.
  vertexArray.addVertexAttrib(2, GL_FLOAT);
  vertexArray.finalizeVertexAttribs();
}

void ScreenQuadMesh::bindTextures(Shader& t_shader,
                                  TextureRegistry* t_textureRegistry) {
  if (textureMaps.empty()) {
    return;
  }

  // Bind textures, assuming a given uniform naming.
  // If a TextureRegistry isn't provided, just start with texture unit 0.
  unsigned int textureUnit = 0;
  if (t_textureRegistry != nullptr) {
    t_textureRegistry->pushUsageBlock();
    textureUnit = t_textureRegistry->getNextTextureUnit();
  }

  Texture& texture = textureMaps[0].getTexture();
  texture.bindToUnit(textureUnit, ETextureBindType::TEXTURE_2D);

  // Set the sampler to the correct texture unit.
  t_shader.setInt("fnk_screenTexture", textureUnit);
  if (t_textureRegistry != nullptr) {
    t_textureRegistry->popUsageBlock();
  }
}
