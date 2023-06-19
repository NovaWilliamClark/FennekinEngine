#include "core/engine.hpp"
#include "debug/logger.hpp"
#include "gui.hpp"


Engine::Engine() : m_window(1920, 1080, "Model Render", false, 4) {
}

Engine::~Engine() = default;

void Engine::run() {

    LOG_INFO("Engine Initialization took: {0}ms", m_timer.endTimer<Time::Milliseconds>());
    LOG_INFO("Entering the main loops");

    // BUG: Does Not display models when moved int an engine class (but works in main) I can't work out why, possibly something to do with member variable initialization order?
    // Initialize Gui
//     IMGUI_CHECKVERSION();
//     ImGui::CreateContext();             // Set ImGui to attach to the OpenGL context
//     const ImGuiIO& io = ImGui::GetIO(); // Get the IO of ImGui
//     ImGui_ImplGlfw_InitForOpenGL(m_window.getGlfwRef(), true);
//     ImGui_ImplOpenGL3_Init("#version 460 core");
//     ModelRenderOptions guiOptions;
// 
//     m_camera = std::make_shared<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));
//     m_cameraController = std::make_shared<OrbitCameraControls>(*m_camera);
//     m_window.bindCamera(m_camera);
//     m_window.bindCameraControls(m_cameraController);
// 
// 
//     // Create Light Registry & Create Light Objects
//     m_lightRegistry = std::make_unique<LightRegistry>();
//     m_lightRegistry->setViewSource(m_camera.get());
//     m_directionalLight = std::make_shared<DirectionalLight>();
//     m_lightRegistry->addLight(m_directionalLight.get());
// 
//     CubeMesh pointLightCube;    // Create a primative cube for point lights
//     SphereMesh spotLightSphere; // Create a primative cube for spotlight lights
// 
//     // Generate Point at random Lights
//     constexpr int NUM_LIGHTS = 5;
//     std::mt19937 gen(std::time(nullptr));
//     std::uniform_real_distribution<float> posDist(-2.f, 2.f);
//     std::uniform_real_distribution<float> colorDist(0.5f, 3.f);
//     for (int i = 0; i < NUM_LIGHTS; i++) {
//         auto light = std::make_shared<PointLight>(glm::vec3(posDist(gen), posDist(gen), posDist(gen)));
// 
//         const glm::vec3 lightColor(colorDist(gen), // between 0.5 and 3.0
//                                    colorDist(gen), // between 0.5 and 3.0
//                                    colorDist(gen)  // between 0.5 and 3.0
//         );
// 
//         light->setDiffuse(lightColor);
//         light->setSpecular(lightColor);
//         constexpr Attenuation ATTENUATION = {.constant = 0.0f, .linear = 0.0f, .quadratic = 1.0f};
//         light->setAttenuation(ATTENUATION);
// 
//         m_lightRegistry->addLight(light.get());
//         m_pointLights.push_back(light);
//     }
// 
// 
//     Framebuffer mainFramebuffer(m_window.getSize());
//     Framebuffer finalFramebuffer(m_window.getSize());
// 
//     auto mainColorAttachment = mainFramebuffer.attachTexture(EBufferType::COLOR_HDR_ALPHA);
//     auto finalColorAttachment = finalFramebuffer.attachTexture(EBufferType::COLOR_ALPHA);
// 
//     // Build the GBuffer & prepare the deferred rendering pipeline.
//     DeferredGeometryPassShader defferedGeometryPassShader;
//     defferedGeometryPassShader.addUniformSource(m_camera);
//     m_gBuffer = std::make_shared<GBuffer>(m_window.getSize());
// 
//     m_lightTextureRegistry = std::make_shared<TextureRegistry>();
//     m_lightTextureRegistry->addTextureSource(m_gBuffer);
// 
//     ScreenQuadMesh screenQuad;
//     ScreenShader gBufferVisualizerShader(ShaderPath("content/shaders/gbuffer_vis.frag"));
//     ScreenShader lightingPassShader(ShaderPath("content/shaders/lighting_pass.frag"));
// 
//     lightingPassShader.addUniformSource(m_camera);
//     lightingPassShader.addUniformSource(m_lightTextureRegistry);
//     lightingPassShader.addUniformSource(m_lightRegistry);
// 
//     // Initialize shadow mapping
//     constexpr int SHADOW_MAP_RESOLUTION = 2048;
//     m_shadowMap = std::make_shared<ShadowMap>(SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION);
//     m_lightTextureRegistry->addTextureSource(m_shadowMap);
// 
//     ShadowMapShader shadowShader;
//     m_shadowCamera = std::make_shared<ShadowCamera>(m_directionalLight);
//     shadowShader.addUniformSource(m_shadowCamera);
//     lightingPassShader.addUniformSource(m_shadowCamera);
// 
//     // Initialize SSAO
//     SsaoShader ssaoShader;
//     ssaoShader.addUniformSource(m_camera);
// 
//     m_ssaoKernel = std::make_shared<SsaoKernel>();
//     m_ssaoBuffer = std::make_shared<SsaoBuffer>(m_window.getSize());
//     ssaoShader.addUniformSource(m_ssaoKernel);
// 
//     m_ssaoTextureRegistry = std::make_shared<TextureRegistry>();
//     m_ssaoTextureRegistry->addTextureSource(m_gBuffer);
//     m_ssaoTextureRegistry->addTextureSource(m_ssaoKernel);
//     ssaoShader.addUniformSource(m_ssaoTextureRegistry);
// 
//     SsaoBlurShader ssaoBlurShader;
//     m_ssaoBlurredBuffer = std::make_shared<SsaoBuffer>(m_window.getSize());
//     m_lightTextureRegistry->addTextureSource(m_ssaoBlurredBuffer);
// 
//     // Setup post processing.
//     auto bloomPass = std::make_shared<BloomPass>(m_window.getSize());
// 
//     m_postProcessTextureRegistry = std::make_shared<TextureRegistry>();
//     m_postProcessTextureRegistry->addTextureSource(bloomPass);
//     ScreenShader postprocessShader(ShaderPath("content/shaders/post_processing.frag"));
//     postprocessShader.addUniformSource(m_postProcessTextureRegistry);
// 
//     FXAAShader fxaaShader;
// 
//     // Setup skybox and IBL.
//     SkyboxShader skyboxShader;
//     skyboxShader.addUniformSource(m_camera);
// 
//     constexpr int CUBEMAP_SIZE = 1024;
//     EquirectCubemapConverter equirectCubemapConverter(CUBEMAP_SIZE, CUBEMAP_SIZE, /*generateMips=*/true);
// 
//     // Irradiance map averages radiance uniformly so it doesn't have a lot of high
//     // frequency details and can thus be small.
//     auto irradianceCalculator = std::make_shared<CubemapIrradianceCalculator>(16, 16);
//     auto irradianceMap = irradianceCalculator->getIrradianceMap();
//     m_lightTextureRegistry->addTextureSource(irradianceCalculator);
// 
//     // Create prefiltered envmap for specular IBL. It doesn't have to be super
//     // large.
//     m_prefilteredEnvMapCalculator = std::make_shared<GGXPrefilteredEnvMapCalculator>(CUBEMAP_SIZE, CUBEMAP_SIZE);
//     auto prefilteredEnvMap = m_prefilteredEnvMapCalculator->getPrefilteredEnvMap();
//     m_lightTextureRegistry->addTextureSource(m_prefilteredEnvMapCalculator);
//     lightingPassShader.addUniformSource(m_prefilteredEnvMapCalculator);
// 
//     auto brdfLUT = std::make_shared<GGXBrdfIntegrationCalculator>(CUBEMAP_SIZE, CUBEMAP_SIZE);
//     {
//         // Only needs to be calculated once up front.
//         DebugGroup debugGroup("BRDF LUT calculation");
//         brdfLUT->draw();
//     }
//     auto brdfIntegrationMap = brdfLUT->getBrdfIntegrationMap();
//     m_lightTextureRegistry->addTextureSource(brdfLUT);
// 
//     SkyboxMesh skybox;
// 
//     // Load the actual env map and generate IBL textures.
//     loadSkyboxImage(guiOptions.skyboxImage, skybox, equirectCubemapConverter, *irradianceCalculator,
//                     *m_prefilteredEnvMapCalculator);
// 
//     // Prepare some debug shaders.
//     Shader normalShader(ShaderPath("content/shaders/model.vert"), ShaderPath("content/shaders/normal.frag"),
//                         ShaderPath("content/shaders/model_normals.geom"));
//     normalShader.addUniformSource(m_camera);
// 
//     Shader lampShader(ShaderPath("content/shaders/model.vert"), ShaderPath("content/shaders/lamp.frag"));
//     lampShader.addUniformSource(m_camera);
// 
//     // Load primary model.
//     std::unique_ptr<Model> model = loadModelOrDefault();
// 
// 
//     m_window.loop([&](float t_deltaTime) {
//         // ImGui logic.
//         ImGui_ImplOpenGL3_NewFrame();
//         ImGui_ImplGlfw_NewFrame();
//         ImGui::NewFrame();
// 
//         m_window.setMouseInputPaused(io.WantCaptureMouse);
//         m_window.setKeyInputPaused(io.WantCaptureKeyboard);
// 
//         const ModelRenderOptions prevOpts = guiOptions;
// 
//         // Initialize with certain current options.
//         guiOptions.speed = m_cameraController->getSpeed();
//         guiOptions.sensitivity = m_cameraController->getSensitivity();
//         guiOptions.fov = m_camera->getFov();
//         guiOptions.nearPlane = m_camera->getNearPlane();
//         guiOptions.farPlane = m_camera->getFarPlane();
// 
//         guiOptions.frameDeltas = m_window.getFrameDeltas();
//         guiOptions.numFrameDeltas = m_window.getNumFrameDeltas();
//         guiOptions.frameDeltasOffset = m_window.getFrameDeltasOffset();
//         guiOptions.avgFPS = m_window.getAvgFps();
// 
//         // Render UI.
//         const UIContext UIContext = {
//             .camera = *m_camera,
//             .shadowMap = *m_shadowMap,
//             .ssaoBuffer = *m_ssaoBlurredBuffer,
//         };
//         renderImGuiUI(guiOptions, UIContext);
// 
//         // Post-process options. Some option values are used later during rendering.
//         model->setModelTransform(
//             glm::scale(glm::mat4_cast(guiOptions.modelRotation), glm::vec3(guiOptions.modelScale)));
// 
//         m_directionalLight->setDiffuse(guiOptions.directionalDiffuse * guiOptions.directionalIntensity);
//         m_directionalLight->setSpecular(guiOptions.directionalSpecular * guiOptions.directionalIntensity);
//         m_directionalLight->setDirection(guiOptions.directionalDirection);
// 
//         m_cameraController->setSpeed(guiOptions.speed);
//         m_cameraController->setSensitivity(guiOptions.sensitivity);
//         m_camera->setFov(guiOptions.fov);
//         m_camera->setNearPlane(guiOptions.nearPlane);
//         m_camera->setFarPlane(guiOptions.farPlane);
// 
//         if (guiOptions.cameraControlType != prevOpts.cameraControlType) {
//             std::shared_ptr<CameraControls> newControls;
//             switch (guiOptions.cameraControlType) {
//             case CameraControlType::FLY:
//                 newControls = std::make_shared<FlyCameraControls>();
//                 break;
//             case CameraControlType::ORBIT:
//                 newControls = std::make_shared<OrbitCameraControls>(*m_camera);
//                 break;
//             }
//             newControls->setSpeed(m_cameraController->getSpeed());
//             newControls->setSensitivity(m_cameraController->getSensitivity());
//             m_cameraController = newControls;
//             m_window.bindCameraControls(m_cameraController);
//         }
// 
//         if (guiOptions.enableVsync != prevOpts.enableVsync) {
//             if (guiOptions.enableVsync) {
//                 m_window.enableVsync();
//             } else {
//                 m_window.disableVsync();
//             }
//         }
// 
//         if (guiOptions.skyboxImage != prevOpts.skyboxImage) {
//             loadSkyboxImage(guiOptions.skyboxImage, skybox, equirectCubemapConverter, *irradianceCalculator,
//                             *m_prefilteredEnvMapCalculator);
//         }
// 
//         m_window.setMouseButtonBehavior(guiOptions.captureMouse ? MouseButtonBehavior::CAPTURE_MOUSE
//                                                                 : MouseButtonBehavior::NONE);
// 
//         // == Main render path ==
//         // Step 0: optional shadow pass.
//         if (guiOptions.shadowMapping) {
//             DebugGroup debugGroup("Directional shadow map");
//             m_shadowCamera->setCuboidExtents(guiOptions.shadowCameraCuboidExtents);
//             m_shadowCamera->setNearPlane(guiOptions.shadowCameraNear);
//             m_shadowCamera->setFarPlane(guiOptions.shadowCameraFar);
//             m_shadowCamera->setDistanceFromOrigin(guiOptions.shadowCameraDistance);
// 
//             m_shadowMap->activate();
//             m_shadowMap->clear();
//             shadowShader.updateUniforms();
//             model->draw(shadowShader);
//             m_shadowMap->deactivate();
//         }
// 
//         // Step 1: geometry pass. Build the G-Buffer.
//         {
//             DebugGroup debugGroup("Geometry pass");
//             m_gBuffer->activate();
//             m_gBuffer->clear();
// 
//             defferedGeometryPassShader.updateUniforms();
// 
//             // Draw model.
//             if (guiOptions.wireframe) {
//                 m_window.enableWireframe();
//             }
//             model->draw(defferedGeometryPassShader);
//             if (guiOptions.wireframe) {
//                 m_window.disableWireframe();
//             }
// 
//             m_gBuffer->deactivate();
//         }
// 
//         if (guiOptions.gBufferVis != GBufferVis::DISABLED) {
//             {
//                 DebugGroup debugGroup("G-Buffer vis");
//                 switch (guiOptions.gBufferVis) {
//                 case GBufferVis::POSITIONS:
//                 case GBufferVis::AO:
//                     screenQuad.setTexture(m_gBuffer->getPositionAOTexture());
//                     break;
//                 case GBufferVis::NORMALS:
//                 case GBufferVis::ROUGHNESS:
//                     screenQuad.setTexture(m_gBuffer->getNormalRoughnessTexture());
//                     break;
//                 case GBufferVis::ALBEDO:
//                 case GBufferVis::METALLIC:
//                     screenQuad.setTexture(m_gBuffer->getAlbedoMetallicTexture());
//                     break;
//                 case GBufferVis::EMISSION:
//                     screenQuad.setTexture(m_gBuffer->getEmissionTexture());
//                     break;
//                 case GBufferVis::DISABLED:
//                     break;
//                 default: ;
//                 }
//                 gBufferVisualizerShader.setInt("gBufferVis", static_cast<int>(guiOptions.gBufferVis));
//                 screenQuad.draw(gBufferVisualizerShader);
//             }
// 
//             // TODO: Refactor avoid needing to copy this.
//             {
//                 DebugGroup debugGroup("Imgui pass");
//                 ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//             }
//             return;
//         }
// 
//         // Step 1.2: optional SSAO pass.
//         // TODO: Extract into an "SSAO pass".
//         if (guiOptions.ssao) {
//             DebugGroup debugGroup("SSAO pass");
//             m_ssaoKernel->setRadius(guiOptions.ssaoRadius);
//             m_ssaoKernel->setBias(guiOptions.ssaoBias);
// 
//             m_ssaoBuffer->activate();
//             m_ssaoBuffer->clear();
// 
//             ssaoShader.updateUniforms();
// 
//             screenQuad.unsetTexture();
//             screenQuad.draw(ssaoShader, m_ssaoTextureRegistry.get());
// 
//             m_ssaoBuffer->deactivate();
// 
//             // Step 1.2.1: SSAO blur.
//             m_ssaoBlurredBuffer->activate();
//             m_ssaoBlurredBuffer->clear();
// 
//             ssaoBlurShader.configureWith(*m_ssaoKernel, *m_ssaoBuffer);
//             screenQuad.draw(ssaoBlurShader);
// 
//             m_ssaoBlurredBuffer->deactivate();
//         }
// 
//         // Step 2: lighting pass. Draw to the main framebuffer.
//         {
//             DebugGroup debugGroup("Deferred lighting pass");
//             mainFramebuffer.activate();
//             mainFramebuffer.clear();
// 
//             // TODO: Set up environment mapping with the skybox.
//             lightingPassShader.updateUniforms();
//             lightingPassShader.setBool("shadowMapping", guiOptions.shadowMapping);
//             lightingPassShader.setFloat("shadowBiasMin", guiOptions.shadowBiasMin);
//             lightingPassShader.setFloat("shadowBiasMax", guiOptions.shadowBiasMax);
//             lightingPassShader.setBool("useIBL", guiOptions.useIBL);
//             lightingPassShader.setBool("ssao", guiOptions.ssao);
//             lightingPassShader.setInt("lightingModel", static_cast<int>(guiOptions.lightingModel));
// 
//             // TODO: Pull this out into a material class.
//             lightingPassShader.setVec3("ambient", guiOptions.ambientColor);
//             lightingPassShader.setFloat("shininess", guiOptions.shininess);
//             lightingPassShader.setFloat("emissionIntensity", guiOptions.emissionIntensity);
//             lightingPassShader.setFloat("emissionAttenuation.constant", guiOptions.emissionAttenuation.x);
//             lightingPassShader.setFloat("emissionAttenuation.linear", guiOptions.emissionAttenuation.y);
//             lightingPassShader.setFloat("emissionAttenuation.quadratic", guiOptions.emissionAttenuation.z);
// 
//             screenQuad.unsetTexture();
//             screenQuad.draw(lightingPassShader, m_lightTextureRegistry.get());
// 
//             mainFramebuffer.deactivate();
//         }
// 
//         // Step 3: forward render anything else on top.
//         {
//             DebugGroup debugGroup("Forward pass");
// 
//             // Before we do so, we have to blit the depth buffer.
//             m_gBuffer->blit(mainFramebuffer, GL_DEPTH_BUFFER_BIT);
// 
//             mainFramebuffer.activate();
// 
//             if (guiOptions.drawNormals) {
//                 // Draw the normals.
//                 normalShader.updateUniforms();
//                 model->draw(normalShader);
//             }
// 
//             // Draw light source.
//             lampShader.updateUniforms();
//             for (const auto& light : m_pointLights) {
//                 pointLightCube.setModelTransform(
//                     glm::scale(glm::translate(glm::mat4(1.0f), light->getPosition()), glm::vec3(0.2f)));
//                 lampShader.setVec3("lightColor", light->getDiffuse());
//                 pointLightCube.draw(lampShader);
//             }
// 
//             // TODO SPOTLIGHT
// 
//             if (guiOptions.wireframe) {
//                 m_window.enableWireframe();
//             }
//             // TODO: Make point lights more part of the UI.
//             // lightSphere.draw(lampShader);
//             if (guiOptions.wireframe) {
//                 m_window.disableWireframe();
//             }
// 
//             // Draw skybox.
//             skyboxShader.updateUniforms();
//             skybox.draw(skyboxShader);
// 
//             mainFramebuffer.deactivate();
//         }
// 
//         // Step 4: post processing.
//         if (guiOptions.bloom) {
//             DebugGroup debugGroup("Bloom pass");
//             bloomPass->multipassDraw(mainFramebuffer);
//         }
// 
//         {
//             DebugGroup debugGroup("Tonemap & gamma");
//             finalFramebuffer.activate();
//             finalFramebuffer.clear();
// 
//             // Draw to the final FB using the post process shader.
//             postprocessShader.updateUniforms();
//             postprocessShader.setBool("bloom", guiOptions.bloom);
//             postprocessShader.setFloat("bloomMix", guiOptions.bloomMix);
//             postprocessShader.setInt("toneMapping", static_cast<int>(guiOptions.toneMapping));
//             postprocessShader.setBool("gammaCorrect", guiOptions.gammaCorrect);
//             postprocessShader.setFloat("gamma", static_cast<int>(guiOptions.gamma));
//             screenQuad.setTexture(mainColorAttachment);
//             screenQuad.draw(postprocessShader, m_postProcessTextureRegistry.get());
// 
//             finalFramebuffer.deactivate();
//         }
// 
//         m_window.setViewport();
// 
//         // Finally draw to the screen via the FXAA shader.
//         if (guiOptions.fxaa) {
//             DebugGroup debugGroup("FXAA");
//             screenQuad.setTexture(finalColorAttachment);
//             screenQuad.draw(fxaaShader);
//         } else {
//             finalFramebuffer.blitToDefault(GL_COLOR_BUFFER_BIT);
//         }
// 
//         // == End render path ==
// 
//         // Finally, draw ImGui data.
//         {
//             DebugGroup debugGroup("Imgui pass");
//             ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//         }
//     });
// 
//     // Cleanup.
//     ImGui_ImplOpenGL3_Shutdown();
//     ImGui_ImplGlfw_Shutdown();
//     ImGui::DestroyContext();
}