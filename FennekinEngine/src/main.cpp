#include "core/gui.hpp"

int main() {

    // TODO Move to Engine Class.

    Window win(1920, 1080, "Model Render", false, 0);
    win.setClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
    win.setEscBehavior(EscBehavior::UNCAPTURE_MOUSE_OR_CLOSE);

    // Setup Dear ImGui.
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(win.getGlfwRef(), /*install_callbacks=*/true);
    ImGui_ImplOpenGL3_Init("#version 460 core");

    // == Main setup ==

    // Prepare opts for usage.
    ModelRenderOptions opts;

    // Setup the camera.
    auto camera = std::make_shared<Camera>(/* position */ glm::vec3(0.0f, 0.0f, 3.0f));
    std::shared_ptr<CameraControls> cameraControls = std::make_shared<OrbitCameraControls>(*camera);
    win.bindCamera(camera);
    win.bindCameraControls(cameraControls);

    // Create light registry and add lights.
    auto lightRegistry = std::make_shared<LightRegistry>();
    lightRegistry->setViewSource(camera.get());

    auto directionalLight = std::make_shared<DirectionalLight>();
    lightRegistry->addLight(directionalLight.get());


    std::mt19937 gen(std::time(nullptr));
    // Create random lights.
    CubeMesh pointLightCube;
    std::vector<std::shared_ptr<PointLight>> pointLights;
    constexpr int NUM_LIGHTS = 5;
    std::uniform_real_distribution<float> posDist(-2.f, 2.f);
    std::uniform_real_distribution<float> lightDist(-0.5f, 3.f);
    for (int i = 0; i < NUM_LIGHTS; i++) {
        auto light = std::make_shared<PointLight>(glm::vec3(posDist(gen), posDist(gen), posDist(gen)));

        glm::vec3 lightColor(lightDist(gen), // between 0.5 and 3.0
                             lightDist(gen), // between 0.5 and 3.0
                             lightDist(gen)  // between 0.5 and 3.0
        );

        light->setDiffuse(lightColor);
        light->setSpecular(lightColor);
        Attenuation attenuation = {.constant = 0.0f, .linear = 0.0f, .quadratic = 1.0f};
        light->setAttenuation(attenuation);

        lightRegistry->addLight(light.get());
        pointLights.push_back(light);
    }

    SphereMesh spotLightSphere;
    std::vector<std::shared_ptr<SpotLight>> spotLights;
    for (int i = 0; i < NUM_LIGHTS; i++) {
        auto light = std::make_shared<SpotLight>(glm::vec3(posDist(gen), posDist(gen), posDist(gen)));

        glm::vec3 lightColor(lightDist(gen), // between 0.5 and 3.0
                             lightDist(gen), // between 0.5 and 3.0
                             lightDist(gen)  // between 0.5 and 3.0
        );

        light->setDiffuse(lightColor);
        light->setSpecular(lightColor);
        Attenuation attenuation = {.constant = 0.0f, .linear = 0.0f, .quadratic = 1.0f};
        light->setAttenuation(attenuation);

        lightRegistry->addLight(light.get());
        spotLights.push_back(light);
    }

    // Set up the main framebuffer that will store intermediate states.
    Framebuffer mainFb(win.getSize());
    auto mainColorAttachment = mainFb.attachTexture(EBufferType::COLOR_HDR_ALPHA);
    mainFb.attachRenderbuffer(EBufferType::DEPTH_AND_STENCIL);

    Framebuffer finalFb(win.getSize());
    auto finalColorAttachment = finalFb.attachTexture(EBufferType::COLOR_ALPHA);

    // Build the G-Buffer and prepare deferred shading.
    DeferredGeometryPassShader geometryPassShader;
    geometryPassShader.addUniformSource(camera);

    auto gBuffer = std::make_shared<GBuffer>(win.getSize());
    auto lightingTextureRegistry = std::make_shared<TextureRegistry>();
    lightingTextureRegistry->addTextureSource(gBuffer);

    ScreenQuadMesh screenQuad;
    ScreenShader gBufferVisShader(ShaderPath("content/shaders/gbuffer_vis.frag"));

    ScreenShader lightingPassShader(ShaderPath("content/shaders/lighting_pass.frag"));
    lightingPassShader.addUniformSource(camera);
    lightingPassShader.addUniformSource(lightingTextureRegistry);
    lightingPassShader.addUniformSource(lightRegistry);

    // Setup shadow mapping.
    constexpr int SHADOW_MAP_SIZE = 2048;
    auto shadowMap = std::make_shared<ShadowMap>(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
    lightingTextureRegistry->addTextureSource(shadowMap);

    ShadowMapShader shadowShader;
    auto shadowCamera = std::make_shared<ShadowCamera>(directionalLight);
    shadowShader.addUniformSource(shadowCamera);
    lightingPassShader.addUniformSource(shadowCamera);

    // Setup SSAO.
    SsaoShader ssaoShader;
    ssaoShader.addUniformSource(camera);

    auto ssaoKernel = std::make_shared<SsaoKernel>();
    ssaoShader.addUniformSource(ssaoKernel);
    auto ssaoBuffer = std::make_shared<SsaoBuffer>(win.getSize());

    auto ssaoTextureRegistry = std::make_shared<TextureRegistry>();
    ssaoTextureRegistry->addTextureSource(gBuffer);
    ssaoTextureRegistry->addTextureSource(ssaoKernel);
    ssaoShader.addUniformSource(ssaoTextureRegistry);

    SsaoBlurShader ssaoBlurShader;
    auto ssaoBlurredBuffer = std::make_shared<SsaoBuffer>(win.getSize());
    lightingTextureRegistry->addTextureSource(ssaoBlurredBuffer);

    // Setup post processing.
    auto bloomPass = std::make_shared<BloomPass>(win.getSize());

    auto postprocessTextureRegistry = std::make_shared<TextureRegistry>();
    postprocessTextureRegistry->addTextureSource(bloomPass);
    ScreenShader postprocessShader(ShaderPath("content/shaders/post_processing.frag"));
    postprocessShader.addUniformSource(postprocessTextureRegistry);

    FXAAShader fxaaShader;

    // Setup skybox and IBL.
    SkyboxShader skyboxShader;
    skyboxShader.addUniformSource(camera);

    constexpr int CUBEMAP_SIZE = 1024;
    EquirectCubemapConverter equirectCubemapConverter(CUBEMAP_SIZE, CUBEMAP_SIZE, /*generateMips=*/true);

    // Irradiance map averages radiance uniformly so it doesn't have a lot of high
    // frequency details and can thus be small.
    auto irradianceCalculator = std::make_shared<CubemapIrradianceCalculator>(16, 16);
    auto irradianceMap = irradianceCalculator->getIrradianceMap();
    lightingTextureRegistry->addTextureSource(irradianceCalculator);

    // Create prefiltered envmap for specular IBL. It doesn't have to be super
    // large.
    auto prefilteredEnvMapCalculator = std::make_shared<GGXPrefilteredEnvMapCalculator>(CUBEMAP_SIZE, CUBEMAP_SIZE);
    auto prefilteredEnvMap = prefilteredEnvMapCalculator->getPrefilteredEnvMap();
    lightingTextureRegistry->addTextureSource(prefilteredEnvMapCalculator);
    lightingPassShader.addUniformSource(prefilteredEnvMapCalculator);

    auto brdfLUT = std::make_shared<GGXBrdfIntegrationCalculator>(CUBEMAP_SIZE, CUBEMAP_SIZE);
    {
        // Only needs to be calculated once up front.
        DebugGroup debugGroup("BRDF LUT calculation");
        brdfLUT->draw();
    }
    auto brdfIntegrationMap = brdfLUT->getBrdfIntegrationMap();
    lightingTextureRegistry->addTextureSource(brdfLUT);

    SkyboxMesh skybox;

    // Load the actual env map and generate IBL textures.
    loadSkyboxImage(opts.skyboxImage, skybox, equirectCubemapConverter, *irradianceCalculator,
                    *prefilteredEnvMapCalculator);

    // Prepare some debug shaders.
    Shader normalShader(ShaderPath("content/shaders/model.vert"), ShaderPath("content/shaders/normal.frag"),
                        ShaderPath("content/shaders/model_normals.geom"));
    normalShader.addUniformSource(camera);

    Shader lampShader(ShaderPath("content/shaders/model.vert"), ShaderPath("content/shaders/lamp.frag"));
    lampShader.addUniformSource(camera);

    // Load primary model.
    std::unique_ptr<Model> model = loadModelOrDefault();

    win.enableFaceCull();
    win.loop([&](float deltaTime) {
        // ImGui logic.
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        win.setMouseInputPaused(io.WantCaptureMouse);
        win.setKeyInputPaused(io.WantCaptureKeyboard);

        ModelRenderOptions prevOpts = opts;

        // Initialize with certain current options.
        opts.speed = cameraControls->getSpeed();
        opts.sensitivity = cameraControls->getSensitivity();
        opts.fov = camera->getFov();
        opts.nearPlane = camera->getNearPlane();
        opts.farPlane = camera->getFarPlane();

        opts.frameDeltas = win.getFrameDeltas();
        opts.numFrameDeltas = win.getNumFrameDeltas();
        opts.frameDeltasOffset = win.getFrameDeltasOffset();
        opts.avgFPS = win.getAvgFps();

        // Render UI.
        UIContext ctx = {
            .camera = *camera,
            .shadowMap = *shadowMap,
            .ssaoBuffer = *ssaoBlurredBuffer,
        };
        renderImGuiUI(opts, ctx);

        // Post-process options. Some option values are used later during rendering.
        model->setModelTransform(glm::scale(glm::mat4_cast(opts.modelRotation), glm::vec3(opts.modelScale)));

        directionalLight->setDiffuse(opts.directionalDiffuse * opts.directionalIntensity);
        directionalLight->setSpecular(opts.directionalSpecular * opts.directionalIntensity);
        directionalLight->setDirection(opts.directionalDirection);

        cameraControls->setSpeed(opts.speed);
        cameraControls->setSensitivity(opts.sensitivity);
        camera->setFov(opts.fov);
        camera->setNearPlane(opts.nearPlane);
        camera->setFarPlane(opts.farPlane);

        if (opts.cameraControlType != prevOpts.cameraControlType) {
            std::shared_ptr<CameraControls> newControls;
            switch (opts.cameraControlType) {
            case CameraControlType::FLY:
                newControls = std::make_shared<FlyCameraControls>();
                break;
            case CameraControlType::ORBIT:
                newControls = std::make_shared<OrbitCameraControls>(*camera);
                break;
            }
            newControls->setSpeed(cameraControls->getSpeed());
            newControls->setSensitivity(cameraControls->getSensitivity());
            cameraControls = newControls;
            win.bindCameraControls(cameraControls);
        }

        if (opts.enableVsync != prevOpts.enableVsync) {
            if (opts.enableVsync) {
                win.enableVsync();
            } else {
                win.disableVsync();
            }
        }

        if (opts.skyboxImage != prevOpts.skyboxImage) {
            loadSkyboxImage(opts.skyboxImage, skybox, equirectCubemapConverter, *irradianceCalculator,
                            *prefilteredEnvMapCalculator);
        }

        win.setMouseButtonBehavior(opts.captureMouse ? MouseButtonBehavior::CAPTURE_MOUSE : MouseButtonBehavior::NONE);

        // == Main render path ==
        // Step 0: optional shadow pass.
        if (opts.shadowMapping) {
            DebugGroup debugGroup("Directional shadow map");
            shadowCamera->setCuboidExtents(opts.shadowCameraCuboidExtents);
            shadowCamera->setNearPlane(opts.shadowCameraNear);
            shadowCamera->setFarPlane(opts.shadowCameraFar);
            shadowCamera->setDistanceFromOrigin(opts.shadowCameraDistance);

            shadowMap->activate();
            shadowMap->clear();
            shadowShader.updateUniforms();
            model->draw(shadowShader);
            shadowMap->deactivate();
        }

        // Step 1: geometry pass. Build the G-Buffer.
        {
            DebugGroup debugGroup("Geometry pass");
            gBuffer->activate();
            gBuffer->clear();

            geometryPassShader.updateUniforms();

            // Draw model.
            if (opts.wireframe) {
                win.enableWireframe();
            }
            model->draw(geometryPassShader);
            if (opts.wireframe) {
                win.disableWireframe();
            }

            gBuffer->deactivate();
        }

        if (opts.gBufferVis != GBufferVis::DISABLED) {
            {
                DebugGroup debugGroup("G-Buffer vis");
                switch (opts.gBufferVis) {
                case GBufferVis::POSITIONS:
                case GBufferVis::AO:
                    screenQuad.setTexture(gBuffer->getPositionAOTexture());
                    break;
                case GBufferVis::NORMALS:
                case GBufferVis::ROUGHNESS:
                    screenQuad.setTexture(gBuffer->getNormalRoughnessTexture());
                    break;
                case GBufferVis::ALBEDO:
                case GBufferVis::METALLIC:
                    screenQuad.setTexture(gBuffer->getAlbedoMetallicTexture());
                    break;
                case GBufferVis::EMISSION:
                    screenQuad.setTexture(gBuffer->getEmissionTexture());
                    break;
                case GBufferVis::DISABLED:
                    break;
                default: ;
                }
                gBufferVisShader.setInt("gBufferVis", static_cast<int>(opts.gBufferVis));
                screenQuad.draw(gBufferVisShader);
            }

            // TODO: Refactor avoid needing to copy this.
            {
                DebugGroup debugGroup("Imgui pass");
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            }
            return;
        }

        // Step 1.2: optional SSAO pass.
        // TODO: Extract into an "SSAO pass".
        if (opts.ssao) {
            DebugGroup debugGroup("SSAO pass");
            ssaoKernel->setRadius(opts.ssaoRadius);
            ssaoKernel->setBias(opts.ssaoBias);

            ssaoBuffer->activate();
            ssaoBuffer->clear();

            ssaoShader.updateUniforms();

            screenQuad.unsetTexture();
            screenQuad.draw(ssaoShader, ssaoTextureRegistry.get());

            ssaoBuffer->deactivate();

            // Step 1.2.1: SSAO blur.
            ssaoBlurredBuffer->activate();
            ssaoBlurredBuffer->clear();

            ssaoBlurShader.configureWith(*ssaoKernel, *ssaoBuffer);
            screenQuad.draw(ssaoBlurShader);

            ssaoBlurredBuffer->deactivate();
        }

        // Step 2: lighting pass. Draw to the main framebuffer.
        {
            DebugGroup debugGroup("Deferred lighting pass");
            mainFb.activate();
            mainFb.clear();

            // TODO: Set up environment mapping with the skybox.
            lightingPassShader.updateUniforms();
            lightingPassShader.setBool("shadowMapping", opts.shadowMapping);
            lightingPassShader.setFloat("shadowBiasMin", opts.shadowBiasMin);
            lightingPassShader.setFloat("shadowBiasMax", opts.shadowBiasMax);
            lightingPassShader.setBool("useIBL", opts.useIBL);
            lightingPassShader.setBool("ssao", opts.ssao);
            lightingPassShader.setInt("lightingModel", static_cast<int>(opts.lightingModel));

            // TODO: Pull this out into a material class.
            lightingPassShader.setVec3("ambient", opts.ambientColor);
            lightingPassShader.setFloat("shininess", opts.shininess);
            lightingPassShader.setFloat("emissionIntensity", opts.emissionIntensity);
            lightingPassShader.setFloat("emissionAttenuation.constant", opts.emissionAttenuation.x);
            lightingPassShader.setFloat("emissionAttenuation.linear", opts.emissionAttenuation.y);
            lightingPassShader.setFloat("emissionAttenuation.quadratic", opts.emissionAttenuation.z);

            screenQuad.unsetTexture();
            screenQuad.draw(lightingPassShader, lightingTextureRegistry.get());

            mainFb.deactivate();
        }

        // Step 3: forward render anything else on top.
        {
            DebugGroup debugGroup("Forward pass");

            // Before we do so, we have to blit the depth buffer.
            gBuffer->blit(mainFb, GL_DEPTH_BUFFER_BIT);

            mainFb.activate();

            if (opts.drawNormals) {
                // Draw the normals.
                normalShader.updateUniforms();
                model->draw(normalShader);
            }

            // Draw light source.
            lampShader.updateUniforms();
            for (const auto& light : pointLights) {
                pointLightCube.setModelTransform(
                    glm::scale(glm::translate(glm::mat4(1.0f), light->getPosition()), glm::vec3(0.2f)));
                lampShader.setVec3("lightColor", light->getDiffuse());
                pointLightCube.draw(lampShader);
            }

            lampShader.updateUniforms();
            for (const auto& light : spotLights) {
                spotLightSphere.setModelTransform(
                    glm::scale(glm::translate(glm::mat4(1.0f), light->getPosition()), glm::vec3(0.05f)));
                lampShader.setVec3("lightColor", light->getDiffuse());
                spotLightSphere.draw(lampShader);
            }

            if (opts.wireframe) {
                win.enableWireframe();
            }
            // TODO: Make point lights more part of the UI.
            // lightSphere.draw(lampShader);
            if (opts.wireframe) {
                win.disableWireframe();
            }

            // Draw skybox.
            skyboxShader.updateUniforms();
            skybox.draw(skyboxShader);

            mainFb.deactivate();
        }

        // Step 4: post processing.
        if (opts.bloom) {
            DebugGroup debugGroup("Bloom pass");
            bloomPass->multipassDraw(/*sourceFb=*/mainFb);
        }

        {
            DebugGroup debugGroup("Tonemap & gamma");
            finalFb.activate();
            finalFb.clear();

            // Draw to the final FB using the post process shader.
            postprocessShader.updateUniforms();
            postprocessShader.setBool("bloom", opts.bloom);
            postprocessShader.setFloat("bloomMix", opts.bloomMix);
            postprocessShader.setInt("toneMapping", static_cast<int>(opts.toneMapping));
            postprocessShader.setBool("gammaCorrect", opts.gammaCorrect);
            postprocessShader.setFloat("gamma", static_cast<int>(opts.gamma));
            screenQuad.setTexture(mainColorAttachment);
            screenQuad.draw(postprocessShader, postprocessTextureRegistry.get());

            finalFb.deactivate();
        }

        win.setViewport();

        // Finally draw to the screen via the FXAA shader.
        if (opts.fxaa) {
            DebugGroup debugGroup("FXAA");
            screenQuad.setTexture(finalColorAttachment);
            screenQuad.draw(fxaaShader);
        } else {
            finalFb.blitToDefault(GL_COLOR_BUFFER_BIT);
        }

        // == End render path ==

        // Finally, draw ImGui data.
        {
            DebugGroup debugGroup("Imgui pass");
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }
    });

    // Cleanup.
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    return 0;
}
