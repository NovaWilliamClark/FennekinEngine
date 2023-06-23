#pragma once

#include "core/debug/exceptions.hpp"
#include "rendering/resources/shader.hpp"
#include "rendering/resources/texture.hpp"

#include <string>
#include <vector>


class TextureSource {
 public:
    virtual ~TextureSource() = default;
    // Binds one or more textures to the next available texture unit, and
  // assigns shader uniforms. It must return the next texture unit that can be
  // used.
  // TODO: Maybe instead allow the source to call getNextTextureUnit().
  virtual unsigned int bindTexture(unsigned int t_nextTextureUnit, Shader& t_shader) = 0;
};

// A manager of "texture-like" objects, in relation to how they are rendered.
// Acts as a uniform source that also binds any texture sources registered with
// it.
//
// Rendering code should set up any textures that won't change between draw
// calls (such as shadow maps) as part of a TextureSource added to this
// registry. Then for each draw call, code should push a usage block, call
// getNextTextureUnit repeatedly to set up texture, and then pop once done.
class TextureRegistry final : public UniformSource {
 public:
    ~TextureRegistry() override = default;

  void addTextureSource(const std::shared_ptr<TextureSource>& t_source) {
    m_textureSources.push_back(t_source);
  }
  void updateUniforms(Shader& t_shader) override;

  // Returns the next available texture unit, and increments the number.
  unsigned int getNextTextureUnit() { return m_nextTextureUnit++; }

  // Remembers the next available texture unit, for a block of usage (such as a
  // draw call).
  void pushUsageBlock();

  // Resets the next available texture unit to the one at the last
  // pushUsageBlock call.
  void popUsageBlock();

 private:
  unsigned int m_nextTextureUnit = 0;
  std::vector<unsigned int> m_lastAvailableUnits;
  std::vector<std::shared_ptr<TextureSource>> m_textureSources;
};
