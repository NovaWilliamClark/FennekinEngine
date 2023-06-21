#pragma once

#include "core/debug/exceptions.hpp"
#include "rendering/interfaces/screen.hpp"

#include <glm/glm.hpp>
#include <string>
#include <vector>

class TextureException : public QuarkException {
    using QuarkException::QuarkException;
};

// Texture type.
enum class ETextureType {
    TEXTURE_2D = 0,
    CUBEMAP,
};

inline GLenum textureTypeToGlTarget(ETextureType type) {
    switch (type) {
    case ETextureType::TEXTURE_2D:
        return GL_TEXTURE_2D;
    case ETextureType::CUBEMAP:
        return GL_TEXTURE_CUBE_MAP;
    }
    throw TextureException("ERROR::TEXTURE::INVALID_TEXTURE_TYPE\n" + std::to_string(static_cast<int>(type)));
}

// The type of texture binding.
enum class ETextureBindType {
    // By default we bind by texture type.
    BY_TEXTURE_TYPE = 0,
    // A TEXTURE_2D.
    TEXTURE_2D,
    // A cubemap.
    CUBEMAP,
    // An image texture that is directly indexed, rather than sampled.
    IMAGE_TEXTURE,
};

inline ETextureBindType textureTypeToTextureBindType(ETextureType type) {
    switch (type) {
    case ETextureType::TEXTURE_2D:
        return ETextureBindType::TEXTURE_2D;
    case ETextureType::CUBEMAP:
        return ETextureBindType::CUBEMAP;
    }
    throw TextureException("ERROR::TEXTURE::INVALID_TEXTURE_TYPE\n" + std::to_string(static_cast<int>(type)));
}

enum class ETextureFiltering {
    // Uses nearest-neighbor sampling.
    NEAREST = 0,
    // Uses linear interpolation between texels.
    BILINEAR,
    // Uses linear interpolation between mipmap levels and their texels.
    TRILINEAR,
    // Handles anisotropy when sampling.
    ANISOTROPIC,
};

enum class TextureWrapMode {
    REPEAT = 0,
    CLAMP_TO_EDGE,
    CLAMP_TO_BORDER,
    // TODO: Add the others?
};

enum class EMipGeneration {
    // Never generates or reserves mipmaps.
    NEVER = 0,
    // Generates mipmaps when loading, but not when creating empty textures.
    ON_LOAD,
    // Always attempts to generate mipmaps when possible.
    ALWAYS,
};

struct TextureParams {
    // OpenGL texture coordinates start at the bottom-right of the image, so we
    // flip vertically by default.
    bool flipVerticallyOnLoad = true;
    ETextureFiltering filtering = ETextureFiltering::NEAREST;
    TextureWrapMode wrapMode = TextureWrapMode::REPEAT;
    glm::vec4 borderColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    EMipGeneration generateMips = EMipGeneration::ON_LOAD;
    // Maximum number of mips to allocate. If negative, no maximum is used.
    int maxNumMips = -1;
};

// Returns the number of mips for an image of a given width/height.
int calculateNumMips(int width, int height);

// Returns the next mip size given an initial size.
ImageSize calculateNextMip(const ImageSize& mipSize);

// Returns the calculated size for a mip level.
ImageSize calculateMipLevel(int mip0Width, int mip0Height, int level);

class Texture {
public:
    // Loads a texture from a given path.
    // TODO: Consider putting this in a TextureLoader class.
    static Texture load(const char* path, bool isSRGB = true);
    static Texture load(const char* path, bool isSRGB, const TextureParams& params);

    // Loads an HDR texture from the given path.
    static Texture loadHdr(const char* path);
    static Texture loadHdr(const char* path, const TextureParams& params);

    static Texture loadCubemap(std::vector<std::string> faces);
    static Texture loadCubemap(std::vector<std::string> faces, const TextureParams& params);


    static Texture create(int width, int height, GLenum internalFormat);
    static Texture create(int width, int height, GLenum internalFormat, const TextureParams& params);
    static Texture createCubemap(int size, GLenum internalFormat);
    static Texture createCubemap(int size, GLenum internalFormat, const TextureParams& params);
    // Creates a custom texture based on the given input data.
    // TODO: Change this to take an unsigned char ptr?
    static Texture createFromData(int width, int height, GLenum internalFormat, const std::vector<glm::vec3>& data);
    static Texture createFromData(int width, int height, GLenum internalFormat, const std::vector<glm::vec3>& data,
                                  const TextureParams& params);

    // TODO: Replace this with proper RAII.
    void free();

    void bindToUnit(unsigned int textureUnit, ETextureBindType bindType = ETextureBindType::BY_TEXTURE_TYPE);


    void generateMips(int maxNumMips = -1);


    void setSamplerMipRange(int min, int max);
    // Resets the allowed mip range to default values.
    void unsetSamplerMipRange();

    [[nodiscard]] unsigned int getId() const {
        return m_id;
    }
    [[nodiscard]] ETextureType getType() const {
        return m_type;
    }
    [[nodiscard]] [[nodiscard]] std::string getPath() const {
        return m_path;
    }
    [[nodiscard]] int getWidth() const {
        return m_width;
    }
    [[nodiscard]] int getHeight() const {
        return m_height;
    }
    [[nodiscard]] int getNumChannels() const {
        return m_numChannels;
    }
    [[nodiscard]] int getNumMips() const {
        return m_numMips;
    }
    // TODO: Remove GLenum from this API (use a custom enum).
    [[nodiscard]] GLenum getInternalFormat() const {
        return m_internalFormat;
    }

private:
    // TODO: Texture lifetimes aren't managed currently, so they aren't unloaded.
    unsigned int m_id;
    ETextureType m_type;
    std::string m_path;
    int m_width;
    int m_height;
    int m_numChannels;
    int m_numMips;
    GLenum m_internalFormat;

    // Applies the given params to the currently-active texture.
    static void applyParams(const TextureParams& params, ETextureType type = ETextureType::TEXTURE_2D);

    friend class Framebuffer;
    friend class Attachment;
};