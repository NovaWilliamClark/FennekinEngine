#pragma once

#include "rendering/resources/shader.h"
#include "rendering/resources/texture.h"
#include "core/window.h"

#include <glm/glm.hpp>
#include <string>
#include <vector>

class FramebufferException final : public QuarkException {
  using QuarkException::QuarkException;
};

enum class EAttachmentTarget {
  TEXTURE,
  RENDER_BUFFER,
};

enum class EBufferType {
  COLOR = 0,
  // HDR color attachment, allowing color values to exceed 1.0
  COLOR_HDR,
  // SNORM attachment, only allowing numbers in the range [-1, 1] but with
  // greater precision.
  COLOR_SNORM,
  COLOR_ALPHA,
  COLOR_HDR_ALPHA,
  COLOR_SNORM_ALPHA,
  COLOR_CUBEMAP_HDR,
  COLOR_CUBEMAP_HDR_ALPHA,
  GRAYSCALE,
  DEPTH,
  STENCIL,
  DEPTH_AND_STENCIL,
};

class Attachment {
 public:
  // TODO: Make these private.
  unsigned int id;
  int width;
  int height;
  int numMips;
  EAttachmentTarget target;
  EBufferType type;
  // Attachment index. Only applies for color buffers.
  int colorAttachmentIndex;
  ETextureType textureType;

  Texture asTexture();

  // TODO: Replace with Texture?
};

inline GLenum bufferTypeToGlAttachmentType(EBufferType t_type, const int attachmentIndex) {
  switch (t_type) {
    case EBufferType::COLOR:
    case EBufferType::COLOR_HDR:
    case EBufferType::COLOR_SNORM:
    case EBufferType::COLOR_ALPHA:
    case EBufferType::COLOR_HDR_ALPHA:
    case EBufferType::COLOR_SNORM_ALPHA:
    case EBufferType::COLOR_CUBEMAP_HDR:
    case EBufferType::COLOR_CUBEMAP_HDR_ALPHA:
    case EBufferType::GRAYSCALE:
      return GL_COLOR_ATTACHMENT0 + attachmentIndex;
    case EBufferType::DEPTH:
      return GL_DEPTH_ATTACHMENT;
    case EBufferType::STENCIL:
      return GL_STENCIL_ATTACHMENT;
    case EBufferType::DEPTH_AND_STENCIL:
      return GL_DEPTH_STENCIL_ATTACHMENT;
  }
  throw FramebufferException("ERROR::FRAMEBUFFER::INVALID_BUFFER_TYPE\n" +
                             std::to_string(static_cast<int>(t_type)));
}

inline GLenum bufferTypeToGlInternalFormat(EBufferType t_type) {
  switch (t_type) {
    case EBufferType::COLOR:
      return GL_RGB8;
    case EBufferType::COLOR_HDR:
    case EBufferType::COLOR_CUBEMAP_HDR:
      return GL_RGB16F;
    case EBufferType::COLOR_SNORM:
      return GL_RGB16_SNORM;
    case EBufferType::COLOR_ALPHA:
      return GL_RGBA8;
    case EBufferType::COLOR_HDR_ALPHA:
    case EBufferType::COLOR_CUBEMAP_HDR_ALPHA:
      return GL_RGBA16F;
    case EBufferType::COLOR_SNORM_ALPHA:
      return GL_RGBA16_SNORM;
    case EBufferType::GRAYSCALE:
      return GL_R8;
    case EBufferType::DEPTH:
      return GL_DEPTH_COMPONENT32F;
    case EBufferType::STENCIL:
      return GL_STENCIL_INDEX8;
    case EBufferType::DEPTH_AND_STENCIL:
      return GL_DEPTH24_STENCIL8;
  }
  throw FramebufferException("ERROR::FRAMEBUFFER::INVALID_BUFFER_TYPE\n" +
                             std::to_string(static_cast<int>(t_type)));
}

inline GLenum bufferTypeToGlFormat(EBufferType t_type) {
  switch (t_type) {
    case EBufferType::COLOR:
    case EBufferType::COLOR_HDR:
    case EBufferType::COLOR_SNORM:
    case EBufferType::COLOR_CUBEMAP_HDR:
      return GL_RGB;
    case EBufferType::COLOR_ALPHA:
    case EBufferType::COLOR_HDR_ALPHA:
    case EBufferType::COLOR_SNORM_ALPHA:
    case EBufferType::COLOR_CUBEMAP_HDR_ALPHA:
      return GL_RGBA;
    case EBufferType::GRAYSCALE:
      return GL_RED;
    case EBufferType::DEPTH:
      return GL_DEPTH_COMPONENT;
    case EBufferType::STENCIL:
      return GL_STENCIL_INDEX;
    case EBufferType::DEPTH_AND_STENCIL:
      return GL_DEPTH_STENCIL;
  }
  throw FramebufferException("ERROR::FRAMEBUFFER::INVALID_BUFFER_TYPE\n" +
                             std::to_string(static_cast<int>(t_type)));
}

inline GLenum bufferTypeToGlDataType(EBufferType t_type) {
  switch (t_type) {
    case EBufferType::COLOR:
    case EBufferType::COLOR_ALPHA:
      return GL_UNSIGNED_BYTE;
    case EBufferType::COLOR_HDR:
    case EBufferType::COLOR_HDR_ALPHA:
    case EBufferType::COLOR_SNORM:
    case EBufferType::COLOR_SNORM_ALPHA:
    case EBufferType::COLOR_CUBEMAP_HDR:
    case EBufferType::COLOR_CUBEMAP_HDR_ALPHA:
      return GL_FLOAT;
    case EBufferType::GRAYSCALE:
      return GL_UNSIGNED_BYTE;
    case EBufferType::DEPTH:
      return GL_FLOAT;
    case EBufferType::STENCIL:
      return GL_UNSIGNED_BYTE;
    case EBufferType::DEPTH_AND_STENCIL:
      return GL_UNSIGNED_INT_24_8;
  }
  throw FramebufferException("ERROR::FRAMEBUFFER::INVALID_BUFFER_TYPE\n" +
                             std::to_string(static_cast<int>(t_type)));
}

// Represents a generated framebuffer
class Framebuffer {
 public:
  Framebuffer(int t_width, int t_height, int t_samples = 0);
  explicit Framebuffer(const ImageSize t_size, const int t_samples = 0)
      : Framebuffer(t_size.width, t_size.height, t_samples) {}
  virtual ~Framebuffer();

  // Activates the current framebuffer. Optionally specify a mipmap level to
  // draw to, and a cubemap face (0 means GL_TEXTURE_CUBE_MAP_POSITIVE_X, etc).
  void activate(int t_mipLevel = 0, int t_cubemapFace = -1);
  // Deactivates the current framebuffer (and activates the default screen
  // framebuffer).
  void deactivate();

  glm::vec4 getClearColor() { return m_clearColor; }
  void setClearColor(const glm::vec4 t_color) { m_clearColor = t_color; }
  void clear();

  ImageSize getSize();

  Attachment attachTexture(EBufferType t_type);
  Attachment attachTexture(EBufferType t_type, const TextureParams& t_params);
  Attachment attachRenderbuffer(EBufferType t_type);

  // Returns the first texture attachment of the given type.
  Attachment getTexture(EBufferType t_type);
  // Returns the first renderbuffer attachment of the given type.
  Attachment getRenderbuffer(EBufferType t_type);

  // Copies the framebuffer to the target.
  // TODO: Swap out GLenum for fnk type.
  void blit(Framebuffer& target, GLenum bits);
  // Copies the framebuffer to the default framebuffer.
  void blitToDefault(GLenum type);

  void enableAlphaBlending() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
  }
  void disableAlphaBlending() { glDisable(GL_BLEND); }

  void enableAdditiveBlending() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBlendEquation(GL_FUNC_ADD);
  }
  void disableAdditiveBlending() { glDisable(GL_BLEND); }

 private:
  unsigned int m_fbo = 0;
  int m_width;
  int m_height;
  int m_samples;
  std::vector<Attachment> m_attachments;

  bool m_hasColorAttachment = false;
  int m_numColorAttachments = 0;
  bool m_hasDepthAttachment = false;
  bool m_hasStencilAttachment = false;
  glm::vec4 m_clearColor = DEFAULT_CLEAR_COLOR;

  Attachment saveAttachment(unsigned int t_id, int t_numMips,
                            EAttachmentTarget t_target, EBufferType t_type,
                            int t_colorAttachmentIndex, ETextureType t_textureType);
  Attachment getAttachment(EAttachmentTarget t_target, EBufferType t_type);
  void checkFlags(EBufferType t_type);
  void updateFlags(EBufferType t_type);
  // Updates the draw and read buffers based on the current flags.
  void updateBufferSources();
};
