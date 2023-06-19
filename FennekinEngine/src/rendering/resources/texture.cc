#include <gl/glew.h>
#include "texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

#include <glm/gtc/type_ptr.hpp>

int calculateNumMips(int width, int height) {
  return 1 + static_cast<int>(std::floor(std::log2(std::max(width, height))));
}

ImageSize calculateNextMip(const ImageSize& mipSize) {
  return ImageSize{.width = std::max(mipSize.width / 2, 1),
                   .height = std::max(mipSize.height / 2, 1)};
}

ImageSize calculateMipLevel(int mip0Width, int mip0Height, int level) {
  ImageSize size = {mip0Width, mip0Height};
  if (level == 0) {
    return size;
  }
  for (int mip = 0; mip < level; ++mip) {
    size = calculateNextMip(size);
  }
  return size;
}

Texture Texture::load(const char* path, bool isSRGB) {
  TextureParams params = {.filtering = ETextureFiltering::ANISOTROPIC,
                          .wrapMode = TextureWrapMode::REPEAT};
  return load(path, isSRGB, params);
}

Texture Texture::load(const char* path, bool isSRGB,
                      const TextureParams& params) {
  Texture texture;
  texture.m_type = ETextureType::TEXTURE_2D;

  stbi_set_flip_vertically_on_load(params.flipVerticallyOnLoad);
  unsigned char* data =
      stbi_load(path, &texture.m_width, &texture.m_height, &texture.m_numChannels,
                /*desired_channels=*/0);

  if (data == nullptr) {
    stbi_image_free(data);
    throw TextureException("ERROR::TEXTURE::LOAD_FAILED\n" + std::string(path));
  }

  GLenum dataFormat;
  if (texture.m_numChannels == 1) {
    texture.m_internalFormat = GL_R8;
    dataFormat = GL_RED;
  } else if (texture.m_numChannels == 2) {
    texture.m_internalFormat = GL_RG8;
    dataFormat = GL_RG;
  } else if (texture.m_numChannels == 3) {
    texture.m_internalFormat = isSRGB ? GL_SRGB8 : GL_RGB8;
    dataFormat = GL_RGB;
  } else if (texture.m_numChannels == 4) {
    texture.m_internalFormat = isSRGB ? GL_SRGB8_ALPHA8 : GL_RGBA8;
    dataFormat = GL_RGBA;
  } else {
    stbi_image_free(data);
    throw TextureException(
        "ERROR::TEXTURE::UNSUPPORTED_TEXTURE_FORMAT\n"
        "Texture '" +
        std::string(path) + "' contained unsupported number of channels: " +
        std::to_string(texture.m_numChannels));
  }

  glGenTextures(1, &texture.m_id);
  glBindTexture(GL_TEXTURE_2D, texture.m_id);

  // TODO: Replace with glTexStorage2D
  glTexImage2D(GL_TEXTURE_2D, /* mipmap level */ 0, texture.m_internalFormat,
               texture.m_width, texture.m_height, 0,
               /* tex data format */ dataFormat, GL_UNSIGNED_BYTE, data);
  if (params.generateMips >= EMipGeneration::ON_LOAD) {
    glGenerateMipmap(GL_TEXTURE_2D);
    texture.m_numMips = calculateNumMips(texture.m_width, texture.m_height);
    // TODO: Take into account params.maxNumMips
  } else {
    texture.m_numMips = 1;
  }

  // Set texture-wrapping/filtering options.
  applyParams(params, texture.m_type);

  stbi_image_free(data);

  return texture;
}

Texture Texture::loadHdr(const char* path) {
  TextureParams params = {.filtering = ETextureFiltering::BILINEAR,
                          .wrapMode = TextureWrapMode::CLAMP_TO_EDGE};
  return loadHdr(path, params);
}

Texture Texture::loadHdr(const char* path, const TextureParams& params) {
  Texture texture;
  texture.m_type = ETextureType::TEXTURE_2D;
  texture.m_numMips = 1;

  stbi_set_flip_vertically_on_load(true);
  float* data = stbi_loadf(path, &texture.m_width, &texture.m_height,
                           &texture.m_numChannels, /*desired_channels=*/0);

  if (data == nullptr) {
    stbi_image_free(data);
    throw TextureException("ERROR::TEXTURE::LOAD_FAILED\n" + std::string(path));
  }

  GLenum dataFormat;
  if (texture.m_numChannels == 1) {
    texture.m_internalFormat = GL_R16F;
    dataFormat = GL_RED;
  } else if (texture.m_numChannels == 2) {
    texture.m_internalFormat = GL_RG16F;
    dataFormat = GL_RG;
  } else if (texture.m_numChannels == 3) {
    texture.m_internalFormat = GL_RGB16F;
    dataFormat = GL_RGB;
  } else if (texture.m_numChannels == 4) {
    texture.m_internalFormat = GL_RGBA16F;
    dataFormat = GL_RGBA;
  } else {
    stbi_image_free(data);
    throw TextureException(
        "ERROR::TEXTURE::UNSUPPORTED_TEXTURE_FORMAT\n"
        "Texture '" +
        std::string(path) + "' contained unsupported number of channels: " +
        std::to_string(texture.m_numChannels));
  }

  glGenTextures(1, &texture.m_id);
  glBindTexture(GL_TEXTURE_2D, texture.m_id);

  // TODO: Replace with glTexStorage2D
  glTexImage2D(GL_TEXTURE_2D, /*mip=*/0, texture.m_internalFormat,
               texture.m_width, texture.m_height, 0,
               /*tex data format=*/dataFormat, GL_FLOAT, data);

  // Set texture-wrapping/filtering options.
  applyParams(params, texture.m_type);

  stbi_image_free(data);

  return texture;
}

Texture Texture::loadCubemap(std::vector<std::string> faces) {
  TextureParams params = {.filtering = ETextureFiltering::BILINEAR,
                          .wrapMode = TextureWrapMode::CLAMP_TO_EDGE};
  return loadCubemap(faces, params);
}

Texture Texture::loadCubemap(std::vector<std::string> faces,
                             const TextureParams& params) {
  if (faces.size() != 6) {
    throw TextureException(
        "ERROR::TEXTURE::INVALID_ARGUMENT\nMust pass exactly 6 faces to "
        "loadCubemap");
  }

  Texture texture;
  texture.m_type = ETextureType::CUBEMAP;
  texture.m_numMips = 1;
  texture.m_internalFormat = GL_RGB8;  // Cubemaps must be RGB.

  glGenTextures(1, &texture.m_id);
  glBindTexture(GL_TEXTURE_CUBE_MAP, texture.m_id);

  int width, height, numChannels;
  bool initialized = false;
  for (unsigned int i = 0; i < faces.size(); i++) {
    unsigned char* data = stbi_load(faces[i].c_str(), &width, &height,
                                    &numChannels, /*desired_channels=*/0);
    // Error handling.
    if (data == nullptr) {
      stbi_image_free(data);
      throw TextureException("ERROR::TEXTURE::LOAD_FAILED\n" + faces[i]);
    }
    if (numChannels != 3) {
      stbi_image_free(data);
      throw TextureException(
          "ERROR::TEXTURE::UNSUPPORTED_TEXTURE_FORMAT\n"
          "Cubemap texture '" +
          faces[i] + "' contained unsupported number of channels: " +
          std::to_string(numChannels));
    }
    if (!initialized) {
      if (width != height) {
        throw TextureException(
            "ERROR::TEXTURE::INVALID_TEXTURE_SIZE\n"
            "Cubemap texture '" +
            faces[i] + "' was not square");
      }
      texture.m_width = width;
      texture.m_height = height;
      texture.m_numChannels = numChannels;
    } else if (width != texture.m_width || height != texture.m_height) {
      throw TextureException(
          "ERROR::TEXTURE::INVALID_TEXTURE_SIZE\n"
          "Cubemap texture '" +
          faces[i] + "' was a different size than the first face");
    }

    // Load into the next cube map texture position.
    // TODO: Replace with glTexStorage2D
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, /*level=*/0,
                 texture.m_internalFormat, width, height, /*border=*/0,
                 /*format=*/GL_RGB, /*type=*/GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
  }

  applyParams(params, texture.m_type);

  return texture;
}

Texture Texture::create(int width, int height, GLenum internalFormat) {
  TextureParams params = {.filtering = ETextureFiltering::BILINEAR,
                          .wrapMode = TextureWrapMode::CLAMP_TO_EDGE};
  return create(width, height, internalFormat, params);
}

Texture Texture::create(int width, int height, GLenum internalFormat,
                        const TextureParams& params) {
  Texture texture;
  texture.m_type = ETextureType::TEXTURE_2D;
  texture.m_width = width;
  texture.m_height = height;
  texture.m_numChannels = 0;  // Default.
  texture.m_numMips = 1;
  if (params.generateMips == EMipGeneration::ALWAYS) {
    texture.m_numMips = calculateNumMips(texture.m_width, texture.m_height);
    if (params.maxNumMips >= 0) {
      texture.m_numMips = std::min(texture.m_numMips, params.maxNumMips);
    }
  }
  texture.m_internalFormat = internalFormat;

  glGenTextures(1, &texture.m_id);
  glBindTexture(GL_TEXTURE_2D, texture.m_id);

  glTexStorage2D(GL_TEXTURE_2D, texture.m_numMips, texture.m_internalFormat,
                 texture.m_width, texture.m_height);

  // Set texture-wrapping/filtering options.
  applyParams(params, texture.m_type);

  return texture;
}

Texture Texture::createCubemap(int size, GLenum internalFormat) {
  TextureParams params = {.filtering = ETextureFiltering::BILINEAR,
                          .wrapMode = TextureWrapMode::CLAMP_TO_EDGE};
  return createCubemap(size, internalFormat, params);
}

Texture Texture::createCubemap(int size, GLenum internalFormat,
                               const TextureParams& params) {
  Texture texture;
  texture.m_type = ETextureType::CUBEMAP;
  texture.m_width = size;
  texture.m_height = size;
  texture.m_numChannels = 0;  // Default.
  texture.m_numMips = 1;
  if (params.generateMips == EMipGeneration::ALWAYS) {
    texture.m_numMips = calculateNumMips(texture.m_width, texture.m_height);
    if (params.maxNumMips >= 0) {
      texture.m_numMips = std::min(texture.m_numMips, params.maxNumMips);
    }
  }
  texture.m_internalFormat = internalFormat;

  glGenTextures(1, &texture.m_id);
  glBindTexture(GL_TEXTURE_CUBE_MAP, texture.m_id);

  glTexStorage2D(GL_TEXTURE_CUBE_MAP, texture.m_numMips, texture.m_internalFormat,
                 texture.m_width, texture.m_height);

  applyParams(params, texture.m_type);

  return texture;
}

Texture Texture::createFromData(int width, int height, GLenum internalFormat,
                                const std::vector<glm::vec3>& data) {
  TextureParams params = {.filtering = ETextureFiltering::BILINEAR,
                          .wrapMode = TextureWrapMode::CLAMP_TO_EDGE};
  return createFromData(width, height, internalFormat, data, params);
}

Texture Texture::createFromData(int width, int height, GLenum internalFormat,
                                const std::vector<glm::vec3>& data,
                                const TextureParams& params) {
  if (data.size() != (width * height)) {
    throw TextureException("ERROR::TEXTURE::INVALID_DATA_SIZE");
  }

  Texture texture = Texture::create(width, height, internalFormat, params);
  // Upload the data.
  glTexSubImage2D(GL_TEXTURE_2D, /*level=*/0, /*xoffset=*/0,
                  /*yoffset=*/0, texture.m_width, texture.m_height,
                  /*format=*/GL_RGB, GL_FLOAT, data.data());
  return texture;
}

void Texture::bindToUnit(unsigned int textureUnit, ETextureBindType bindType) {
  // TODO: Take into account GL_MAX_TEXTURE_UNITS here.
  glActiveTexture(GL_TEXTURE0 + textureUnit);

  if (bindType == ETextureBindType::BY_TEXTURE_TYPE) {
    bindType = textureTypeToTextureBindType(m_type);
  }

  switch (bindType) {
    case ETextureBindType::TEXTURE_2D:
      glBindTexture(GL_TEXTURE_2D, m_id);
      break;
    case ETextureBindType::CUBEMAP:
      glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
      break;
    case ETextureBindType::IMAGE_TEXTURE:
      // Bind image unit.
      glBindImageTexture(textureUnit, m_id, /*level=*/0, /*layered=*/GL_FALSE, 0,
                         GL_READ_WRITE, m_internalFormat);
      break;
    default:
      throw TextureException("ERROR::TEXTURE::INVALID_TEXTURE_BIND_TYPE\n" +
                             std::to_string(static_cast<int>(bindType)));
  }
}

void Texture::setSamplerMipRange(int min, int max) {
  GLenum target = textureTypeToGlTarget(m_type);
  glBindTexture(target, m_id);
  glTexParameteri(target, GL_TEXTURE_BASE_LEVEL, min);
  glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, max);
}

void Texture::unsetSamplerMipRange() {
  // OpenGL defaults.
  setSamplerMipRange(0, 1000);
}

void Texture::free() { glDeleteTextures(1, &m_id); }

void Texture::generateMips(int maxNumMips) {
  if (maxNumMips >= 0) {
    setSamplerMipRange(0, maxNumMips);
  }

  GLenum target = textureTypeToGlTarget(m_type);
  glBindTexture(target, m_id);
  glGenerateMipmap(target);

  if (maxNumMips >= 0) {
    unsetSamplerMipRange();
  }
}

void Texture::applyParams(const TextureParams& params, ETextureType type) {
  GLenum target = textureTypeToGlTarget(type);

  switch (params.filtering) {
    case ETextureFiltering::NEAREST:
      glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      break;
    case ETextureFiltering::BILINEAR:
      glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      break;
    case ETextureFiltering::TRILINEAR:
    case ETextureFiltering::ANISOTROPIC:
      glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      if (params.filtering == ETextureFiltering::ANISOTROPIC) {
        constexpr float MAX_ANISOTROPY_SAMPLES = 4.0f;
        glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY,
                        MAX_ANISOTROPY_SAMPLES);
      }
      break;
  }

  switch (params.wrapMode) {
    case TextureWrapMode::REPEAT:
      glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
      if (type == ETextureType::CUBEMAP) {
        glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_REPEAT);
      }
      break;
    case TextureWrapMode::CLAMP_TO_EDGE:
      glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      if (type == ETextureType::CUBEMAP) {
        glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
      }
      break;
    case TextureWrapMode::CLAMP_TO_BORDER:
      glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
      glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
      if (type == ETextureType::CUBEMAP) {
        glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
      }
      glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR,
                       glm::value_ptr(params.borderColor));
      break;
  }
}
