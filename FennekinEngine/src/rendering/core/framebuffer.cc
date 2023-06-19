#include <gl/glew.h>
#include "framebuffer.h"


Texture Attachment::asTexture() {
  if (target != EAttachmentTarget::TEXTURE) {
    throw FramebufferException("ERROR::FRAMEBUFFER::INVALID_ATTACHMENT_TARGET");
  }
  Texture texture;
  texture.m_id = id;
  texture.m_type = textureType;
  texture.m_width = width;
  texture.m_height = height;
  texture.m_numMips = numMips;
  return texture;
}

Framebuffer::Framebuffer(int t_width, int t_height, int t_samples)
    : m_width(t_width), m_height(t_height), m_samples(t_samples) {
  glGenFramebuffers(1, &m_fbo);
}

Framebuffer::~Framebuffer() {
  glDeleteFramebuffers(1, &m_fbo);
  // TODO: Delete attachments.
}

void Framebuffer::activate(int t_mipLevel, int t_cubemapFace) {
  glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

  // Activate the specified mip level (usually 0).
  for (Attachment& attachment : m_attachments) {
    GLenum attachmentType = bufferTypeToGlAttachmentType(
        attachment.type, attachment.colorAttachmentIndex);

    switch (attachment.target) {
      case EAttachmentTarget::TEXTURE: {
        GLenum target = GL_TEXTURE_2D;
        if (t_cubemapFace >= 0) {
          if (t_cubemapFace >= 6) {
            throw FramebufferException(
                "ERROR::FRAMEBUFFER::CUBEMAP_FACE_OUT_OF_RANGE");
          }
          target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + t_cubemapFace;
        }
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, target,
                               attachment.id, t_mipLevel);
      } break;
      case EAttachmentTarget::RENDER_BUFFER:
        // Perform some checks.
        if (t_mipLevel != 0) {
          // Non-0 mips are only allowed for textures.
          throw FramebufferException(
              "ERROR::FRAMEBUFFER::MIP_ACTIVATED_FOR_RENDERBUFFER");
        }
        if (t_cubemapFace >= 0) {
          // Renderbuffers currently can't be cubemaps.
          throw FramebufferException(
              "ERROR::FRAMEBUFFER::CUBEMAP_FACE_GIVEN_FOR_RENDERBUFFER");
        }
        break;
    }
  }

  ImageSize mipSize = calculateMipLevel(m_width, m_height, t_mipLevel);
  glViewport(0, 0, mipSize.width, mipSize.height);
}

void Framebuffer::deactivate() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

ImageSize Framebuffer::getSize() {
  ImageSize size = {.width = m_width, .height = m_height};
  return size;
}

Attachment Framebuffer::attachTexture(EBufferType t_type) {
  TextureParams params = {.filtering = ETextureFiltering::BILINEAR,
                          .wrapMode = TextureWrapMode::CLAMP_TO_EDGE};
  return attachTexture(t_type, params);
}

Attachment Framebuffer::attachTexture(EBufferType t_type,
                                      const TextureParams& t_params) {
  checkFlags(t_type);
  activate();

  ETextureType textureType = ETextureType::TEXTURE_2D;
  GLenum textureTarget = m_samples ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
  // Special case cubemaps.
  if (t_type == EBufferType::COLOR_CUBEMAP_HDR ||
      t_type == EBufferType::COLOR_CUBEMAP_HDR_ALPHA) {
    textureTarget = GL_TEXTURE_CUBE_MAP;
    textureType = ETextureType::CUBEMAP;
  }

  // Generate texture based on given buffer type.
  // TODO: This does the same thing as the Texture class - should it use that
  // instead?
  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(textureTarget, texture);

  GLenum internalFormat = bufferTypeToGlInternalFormat(t_type);

  int numMips = 1;
  if (t_params.generateMips == EMipGeneration::ALWAYS) {
    numMips = calculateNumMips(m_width, m_height);
    if (t_params.maxNumMips >= 0) {
      numMips = std::min(numMips, t_params.maxNumMips);
    }
  }
  if (m_samples && textureType != ETextureType::CUBEMAP) {
    glTexStorage2DMultisample(textureTarget, m_samples, internalFormat, m_width,
                              m_height,
                              /*fixedsamplelocations=*/GL_TRUE);
  } else {
    glTexStorage2D(textureTarget, numMips, internalFormat, m_width, m_height);
  }

  Texture::applyParams(t_params, textureType);

  // Attach the texture to the framebuffer.
  int colorAttachmentIndex = m_numColorAttachments;

  GLenum attachmentType =
      bufferTypeToGlAttachmentType(t_type, colorAttachmentIndex);
  GLenum textarget = textureType == ETextureType::CUBEMAP
                         ? GL_TEXTURE_CUBE_MAP_POSITIVE_X
                         : textureTarget;
  glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, textarget, texture,
                         /* mipmap level */ 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    throw FramebufferException("ERROR::FRAMEBUFFER::TEXTURE::INCOMPLETE");
  }

  updateFlags(t_type);
  updateBufferSources();

  glBindTexture(textureTarget, 0);
  deactivate();

  return saveAttachment(texture, numMips, EAttachmentTarget::TEXTURE, t_type,
                        colorAttachmentIndex, textureType);
}

Attachment Framebuffer::attachRenderbuffer(EBufferType t_type) {
  checkFlags(t_type);
  activate();

  // Create and configure renderbuffer.
  // Renderbuffers are similar to textures, but they generally cannot be read
  // from easily. In exchange, their render data is stored in a native format,
  // so they are perfect for use cases that require writing (such as the final
  // frame, or depth/stencil attachments).
  // TODO: Pull out into a renderbuffer class?
  unsigned int rbo;
  glGenRenderbuffers(1, &rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, rbo);

  GLenum internalFormat = bufferTypeToGlInternalFormat(t_type);

  if (m_samples) {
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_samples, internalFormat,
                                     m_width, m_height);
  } else {
    glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, m_width, m_height);
  }

  // Attach the renderbuffer to the framebuffer.
  int colorAttachmentIndex = m_numColorAttachments;
  GLenum attachmentType =
      bufferTypeToGlAttachmentType(t_type, colorAttachmentIndex);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachmentType, GL_RENDERBUFFER,
                            rbo);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    throw FramebufferException("ERROR::FRAMEBUFFER::RENDER_BUFFER::INCOMPLETE");
  }

  updateFlags(t_type);
  updateBufferSources();

  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  deactivate();

  return saveAttachment(rbo, /*numMips=*/1, EAttachmentTarget::RENDER_BUFFER,
                        t_type, colorAttachmentIndex, ETextureType::TEXTURE_2D);
}

Attachment Framebuffer::getTexture(EBufferType t_type) {
  return getAttachment(EAttachmentTarget::TEXTURE, t_type);
}

Attachment Framebuffer::getRenderbuffer(EBufferType t_type) {
  return getAttachment(EAttachmentTarget::RENDER_BUFFER, t_type);
}

void Framebuffer::blit(Framebuffer& target, GLenum bits) {
  // TODO: This doesn't handle non-mip0 blits.
  glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target.m_fbo);
  glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, bits,
                    GL_NEAREST);
  deactivate();
}

void Framebuffer::blitToDefault(GLenum bits) {
  glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, bits,
                    GL_NEAREST);
  deactivate();
}

Attachment Framebuffer::saveAttachment(unsigned int t_id, int t_numMips,
                                       EAttachmentTarget t_target, EBufferType t_type,
                                       int t_colorAttachmentIndex,
                                       ETextureType t_textureType) {
  Attachment attachment = {.id = t_id,
                           .width = m_width,
                           .height = m_height,
                           .numMips = t_numMips,
                           .target = t_target,
                           .type = t_type,
                           .colorAttachmentIndex = t_colorAttachmentIndex,
                           .textureType = t_textureType};
  m_attachments.push_back(attachment);
  return attachment;
}

Attachment Framebuffer::getAttachment(EAttachmentTarget t_target,
                                      EBufferType t_type) {
  for (Attachment& attachment : m_attachments) {
    if (attachment.target == t_target && attachment.type == t_type) {
      return attachment;
    }
  }
  throw FramebufferException("ERROR::FRAMEBUFFER::ATTACHMENT_NOT_FOUND");
}

void Framebuffer::checkFlags(EBufferType t_type) {
  switch (t_type) {
    case EBufferType::COLOR:
    case EBufferType::COLOR_HDR:
    case EBufferType::COLOR_SNORM:
    case EBufferType::COLOR_CUBEMAP_HDR:
    case EBufferType::COLOR_ALPHA:
    case EBufferType::COLOR_HDR_ALPHA:
    case EBufferType::COLOR_SNORM_ALPHA:
    case EBufferType::COLOR_CUBEMAP_HDR_ALPHA:
    case EBufferType::GRAYSCALE:
      // Multiple color attachments OK.
      return;
    case EBufferType::DEPTH:
      if (m_hasDepthAttachment) {
        throw FramebufferException(
            "ERROR::FRAMEBUFFER::BUFFER_TYPE_ALREADY_IN_USE\n" +
            std::to_string(static_cast<int>(t_type)));
      }
      return;
    case EBufferType::STENCIL:
      if (m_hasStencilAttachment) {
        throw FramebufferException(
            "ERROR::FRAMEBUFFER::BUFFER_TYPE_ALREADY_IN_USE\n" +
            std::to_string(static_cast<int>(t_type)));
      }
      return;
    case EBufferType::DEPTH_AND_STENCIL:
      if (m_hasDepthAttachment || m_hasStencilAttachment) {
        throw FramebufferException(
            "ERROR::FRAMEBUFFER::BUFFER_TYPE_ALREADY_IN_USE\n" +
            std::to_string(static_cast<int>(t_type)));
      }
      return;
  }
}

void Framebuffer::updateFlags(EBufferType t_type) {
  switch (t_type) {
    case EBufferType::COLOR:
    case EBufferType::COLOR_HDR:
    case EBufferType::COLOR_SNORM:
    case EBufferType::COLOR_CUBEMAP_HDR:
    case EBufferType::COLOR_ALPHA:
    case EBufferType::COLOR_HDR_ALPHA:
    case EBufferType::COLOR_SNORM_ALPHA:
    case EBufferType::COLOR_CUBEMAP_HDR_ALPHA:
    case EBufferType::GRAYSCALE:
      m_hasColorAttachment = true;
      m_numColorAttachments++;
      return;
    case EBufferType::DEPTH:
      m_hasDepthAttachment = true;
      return;
    case EBufferType::STENCIL:
      m_hasStencilAttachment = true;
      return;
    case EBufferType::DEPTH_AND_STENCIL:
      m_hasDepthAttachment = true;
      m_hasStencilAttachment = true;
      return;
  }
  throw FramebufferException("ERROR::FRAMEBUFFER::INVALID_BUFFER_TYPE\n" +
                             std::to_string(static_cast<int>(t_type)));
}

void Framebuffer::updateBufferSources() {
  if (m_hasColorAttachment) {
    if (m_numColorAttachments == 1) {
      glDrawBuffer(GL_COLOR_ATTACHMENT0);
    } else {
      std::vector<unsigned int> attachments;
      for (int i = 0; i < m_numColorAttachments; i++) {
        attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
      }
      glDrawBuffers(m_numColorAttachments, attachments.data());
    }
    // Always read from attachment 0.
    glReadBuffer(GL_COLOR_ATTACHMENT0);
  } else {
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
  }
}

void Framebuffer::clear() {
  glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);

  GLbitfield clearBits = 0;
  if (m_hasColorAttachment) {
    clearBits |= GL_COLOR_BUFFER_BIT;
  }
  if (m_hasDepthAttachment) {
    clearBits |= GL_DEPTH_BUFFER_BIT;
  }
  if (m_hasStencilAttachment) {
    clearBits |= GL_STENCIL_BUFFER_BIT;
  }
  glClear(clearBits);
}
