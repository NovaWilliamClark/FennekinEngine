#pragma once
#include "window.h"
#include "core/time.hpp"
#include "pch.h"


class Engine
{
public:
    Engine();
    ~Engine();

    void run();

private:
    Time m_timer;


    Window m_window{};

    std::shared_ptr<Camera> m_camera{};
    std::shared_ptr<CameraControls> m_cameraController{};

    // UI

    // Lighting
    std::shared_ptr<LightRegistry> m_lightRegistry{};
    std::shared_ptr<DirectionalLight> m_directionalLight{};
    std::vector<std::shared_ptr<PointLight>> m_pointLights;
    std::vector<std::shared_ptr<SpotLight>> m_spotLights;

    std::shared_ptr<ShadowMap> m_shadowMap{};
    std::shared_ptr<ShadowCamera> m_shadowCamera{};

    // Buffers
    std::shared_ptr<GBuffer> m_gBuffer;

    // Stores
    std::shared_ptr<TextureRegistry> m_lightTextureRegistry{};
    std::shared_ptr<TextureRegistry> m_ssaoTextureRegistry{};
    std::shared_ptr<TextureRegistry> m_postProcessTextureRegistry{};

    // Screen Space Ambient Occlusion
    std::shared_ptr <SsaoKernel> m_ssaoKernel{};
    std::shared_ptr<SsaoBuffer> m_ssaoBuffer{};
    std::shared_ptr<SsaoBuffer> m_ssaoBlurredBuffer{};

    // Irradiance map
    std::shared_ptr<CubemapIrradianceCalculator> m_irradianceCalculator{};
    std::shared_ptr<GGXPrefilteredEnvMapCalculator> m_prefilteredEnvMapCalculator{};


};

