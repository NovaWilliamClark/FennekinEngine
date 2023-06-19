#include "ibl.h"

CubemapIrradianceShader::CubemapIrradianceShader() :
    Shader(ShaderPath("content/shaders/builtin/cubemap.vert"),
           ShaderPath("content/shaders/builtin/irradiance_cubemap.frag")) {
    // Set defaults.
    setHemisphereSampleDelta(m_hemisphereSampleDelta);
}

void CubemapIrradianceShader::setHemisphereSampleDelta(float t_delta) {
    m_hemisphereSampleDelta = t_delta;
    setFloat("fnk_hemisphereSampleDelta", m_hemisphereSampleDelta);
}

CubemapIrradianceCalculator::CubemapIrradianceCalculator(int width, int t_height) :
    m_buffer(width, t_height), m_cubemapRenderHelper(&m_buffer) {
    m_cubemap = m_buffer.attachTexture(EBufferType::COLOR_CUBEMAP_HDR);
}

void CubemapIrradianceCalculator::multipassDraw(Texture t_source) {
    // Set up the source.
    t_source.bindToUnit(0, ETextureBindType::CUBEMAP);
    m_irradianceShader.setInt("fnk_environmentMap", 0);

    m_cubemapRenderHelper.multipassDraw(m_irradianceShader);
}

unsigned int CubemapIrradianceCalculator::bindTexture(unsigned int t_nextTextureUnit, Shader& t_shader) {
    m_cubemap.asTexture().bindToUnit(t_nextTextureUnit, ETextureBindType::CUBEMAP);
    // Bind sampler uniforms.
    t_shader.setInt("fnk_irradianceMap", t_nextTextureUnit);

    return t_nextTextureUnit + 1;
}

GGXPrefilterShader::GGXPrefilterShader() :
    Shader(ShaderPath("content/shaders/builtin/cubemap.vert"),
           ShaderPath("content/shaders/builtin/ggx_prefilter_cubemap.frag")) {
    setNumSamples(m_numSamples);
}

void GGXPrefilterShader::setNumSamples(unsigned int t_samples) {
    m_numSamples = t_samples;
    setUInt("fnk_numSamples", m_numSamples);
}

void GGXPrefilterShader::setRoughness(float t_roughness) {
    setFloat("fnk_roughness", t_roughness);
}

GGXPrefilteredEnvMapCalculator::GGXPrefilteredEnvMapCalculator(int t_width, int t_height, int t_maxNumMips) :
    m_buffer(t_width, t_height), m_cubemapRenderHelper(&m_buffer) {
    TextureParams textureParams = {
        // Need trilinear filtering in order to make use of mip levels.
        .filtering = ETextureFiltering::TRILINEAR,
        // Clamp to the edge so we don't oversample.
        .wrapMode = TextureWrapMode::CLAMP_TO_EDGE,
        // Force mip generation. We can optionally just use a smaller part of the
        // mip chain.
        .generateMips = EMipGeneration::ALWAYS,
        .maxNumMips = t_maxNumMips,
    };
    m_cubemap = m_buffer.attachTexture(EBufferType::COLOR_CUBEMAP_HDR, textureParams);
}

void GGXPrefilteredEnvMapCalculator::multipassDraw(Texture t_source) {
    // Set up the source.
    t_source.bindToUnit(0, ETextureBindType::CUBEMAP);
    m_shader.setInt("fnk_environmentMap", 0);

    for (int mip = 0; mip < m_cubemap.numMips; ++mip) {
        m_cubemapRenderHelper.setTargetMip(mip);
        // Go through roughness from [0..1].
        float roughness = static_cast<float>(mip) / (m_cubemap.numMips - 1);
        m_shader.setRoughness(roughness);
        m_cubemapRenderHelper.multipassDraw(m_shader);
    }
}

void GGXPrefilteredEnvMapCalculator::updateUniforms(Shader& t_shader) {
    t_shader.setFloat("fnk_ggxPrefilteredEnvMapMaxLOD", static_cast<float>(m_cubemap.numMips - 1.0));
}

unsigned int GGXPrefilteredEnvMapCalculator::bindTexture(unsigned int t_nextTextureUnit, Shader& shader) {
    m_cubemap.asTexture().bindToUnit(t_nextTextureUnit, ETextureBindType::CUBEMAP);
    // Bind sampler uniforms.
    shader.setInt("fnk_ggxPrefilteredEnvMap", t_nextTextureUnit);

    return t_nextTextureUnit + 1;
}

GGXBrdfIntegrationShader::GGXBrdfIntegrationShader() :
    ScreenShader(ShaderPath("content/shaders/builtin/ggx_brdf_integration.frag")) {
    setNumSamples(numSamples_);
}

void GGXBrdfIntegrationShader::setNumSamples(unsigned int samples) {
    numSamples_ = samples;
    setUInt("fnk_numSamples", numSamples_);
}

GGXBrdfIntegrationCalculator::GGXBrdfIntegrationCalculator(int width, int height) : m_buffer(width, height) {
    // The BRDF integration map contains values from [0..1] so we can use an SNORM
    // for greater precision.
    // TODO: Use a 2-channel SNORM texture instead.
    m_integrationMap = m_buffer.attachTexture(EBufferType::COLOR_SNORM);
}

void GGXBrdfIntegrationCalculator::draw() {
    m_buffer.activate();
    m_screenQuad.draw(m_shader);
    m_buffer.deactivate();
}

unsigned int GGXBrdfIntegrationCalculator::bindTexture(unsigned int t_nextTextureUnit, Shader& t_shader) {
    m_integrationMap.asTexture().bindToUnit(t_nextTextureUnit);
    // Bind sampler uniforms.
    t_shader.setInt("fnk_ggxIntegrationMap", t_nextTextureUnit);

    return t_nextTextureUnit + 1;
}