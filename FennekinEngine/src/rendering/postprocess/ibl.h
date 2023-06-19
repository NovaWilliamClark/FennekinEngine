#pragma once

#include "rendering/core/framebuffer.h"
#include "rendering/registers/texture_registry.h"
#include "rendering/resources/cubemap.h"
#include "rendering/resources/shader.h"
#include "rendering/resources/shader_primitives.h"
#include "scene/mesh_primitives.h"

class IblException final : public QuarkException {
    using QuarkException::QuarkException;
};

class CubemapIrradianceShader final : public Shader {
public:
    CubemapIrradianceShader();

    [[nodiscard]] float getHemisphereSampleDelta() const {
        return m_hemisphereSampleDelta;
    }
    void setHemisphereSampleDelta(float t_delta);

private:
    float m_hemisphereSampleDelta = 0.025f;
};

// Calculates a diffuse irradiance map based on a given HDR cubemap.
// Note that since each pixel in the irradiance map is uniformly sampled from an
// entire hemisphere, fine details are not preserved - thus the irradiance map
// need not be very large (32x32 can suffice), and the texture's bilinear
// filtering can fill in the blanks.
class CubemapIrradianceCalculator final : public TextureSource {
public:
    explicit CubemapIrradianceCalculator(int width = 32, int t_height = 32);
    explicit CubemapIrradianceCalculator(ImageSize t_size) : CubemapIrradianceCalculator(t_size.width, t_size.height) {
    }
    virtual ~CubemapIrradianceCalculator() = default;

    [[nodiscard]] float getHemisphereSampleDelta() const {
        return m_irradianceShader.getHemisphereSampleDelta();
    }
    void setHemisphereSampleDelta(const float t_delta) {
        m_irradianceShader.setHemisphereSampleDelta(t_delta);
    }

    // Draw onto the allocated cubemap from the given cubemap as the source.
    void multipassDraw(Texture t_source);

    Texture getIrradianceMap() {
        return m_cubemap.asTexture();
    }

    unsigned int bindTexture(unsigned int t_nextTextureUnit, Shader& t_shader) override;

private:
    Framebuffer m_buffer;
    Attachment m_cubemap;
    CubemapIrradianceShader m_irradianceShader;
    CubemapRenderHelper m_cubemapRenderHelper;
};

class GGXPrefilterShader : public Shader {
public:
    GGXPrefilterShader();

    [[nodiscard]] unsigned int getNumSamples() const {
        return m_numSamples;
    }
    void setNumSamples(unsigned int t_samples);

    void setRoughness(float t_roughness);

private:
    unsigned int m_numSamples = 1024;
};

// Calculates the prefiltered env map based on the GGX microfacet model. The map
// contains multiple mip level, which each mip level representing a different
// material roughness (mip0 -> roughness 0).
class GGXPrefilteredEnvMapCalculator : public UniformSource, public TextureSource {
public:
    GGXPrefilteredEnvMapCalculator(int t_width, int t_height, int t_maxNumMips = -1);
    explicit GGXPrefilteredEnvMapCalculator(ImageSize t_size, int t_maxNumMips = -1) :
        GGXPrefilteredEnvMapCalculator(t_size.width, t_size.height, t_maxNumMips) {
    }

    ~GGXPrefilteredEnvMapCalculator() override = default;

    [[nodiscard]] unsigned int getNumSamples() const {
        return m_shader.getNumSamples();
    }
    void setNumSamples(const unsigned int t_samples) {
        m_shader.setNumSamples(t_samples);
    }

    // Draw onto the allocated prefiltering cubemap from the given cubemap as the
    // source. The cubemap should ideally have mip levels in order to avoid
    // hotspot artifacts.
    void multipassDraw(Texture t_source);

    Texture getPrefilteredEnvMap() {
        return m_cubemap.asTexture();
    }

    void updateUniforms(Shader& t_shader) override;
    unsigned int bindTexture(unsigned int t_nextTextureUnit, Shader& shader) override;

private:
    Framebuffer m_buffer;
    Attachment m_cubemap;
    GGXPrefilterShader m_shader;
    CubemapRenderHelper m_cubemapRenderHelper;
};

class GGXBrdfIntegrationShader : public ScreenShader {
public:
    GGXBrdfIntegrationShader();

    [[nodiscard]] unsigned int getNumSamples() const {
        return numSamples_;
    }
    void setNumSamples(unsigned int samples);

private:
    unsigned int numSamples_ = 1024;
};

// Calculates an integration map for the GGX BRDF, parameterized over roughness
// and NdotV.
class GGXBrdfIntegrationCalculator : public TextureSource {
public:
    GGXBrdfIntegrationCalculator(int width, int height);
    explicit GGXBrdfIntegrationCalculator(ImageSize size) : GGXBrdfIntegrationCalculator(size.width, size.height) {
    }
    virtual ~GGXBrdfIntegrationCalculator() = default;

    [[nodiscard]] unsigned int getNumSamples() const {
        return m_shader.getNumSamples();
    }
    void setNumSamples(const unsigned int t_samples) {
        m_shader.setNumSamples(t_samples);
    }

    // Draw onto the allocated BRDF integration texture. This is solely a function
    // of the BRDF and does not require any source data.
    void draw();

    Texture getBrdfIntegrationMap() {
        return m_integrationMap.asTexture();
    }

    unsigned int bindTexture(unsigned int t_nextTextureUnit, Shader& t_shader) override;

private:
    Framebuffer m_buffer;
    Attachment m_integrationMap;
    ScreenQuadMesh m_screenQuad;
    GGXBrdfIntegrationShader m_shader;
};
