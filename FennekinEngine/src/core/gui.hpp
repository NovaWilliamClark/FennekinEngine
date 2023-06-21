#pragma once
#include "debug/debug.hpp"
#include "pch.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imGuIZMOquat.h"
#include "imgui.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include <imgui/backends/imgui_impl_glfw.h>


enum class CameraControlType {
    FLY = 0,
    ORBIT,
};

enum class LightingModel {
    BLINN_PHONG = 0,
    COOK_TORRANCE_GGX,
};

enum class SkyboxImage {
    ALEXS_APT = 0,
    FROZEN_WATERFALL,
    KLOPPENHEIM,
    MILKYWAY,
    MON_VALLEY,
    UENO_SHRINE,
    WINTER_FOREST,

    // TODO: Add the rest of them
};

enum class GBufferVis { DISABLED = 0, POSITIONS, AO, NORMALS, ROUGHNESS, ALBEDO, METALLIC, EMISSION, SSAO };

enum class ToneMapping {
    NONE = 0,
    REINHARD,
    REINHARD_LUMINANCE,
    ACES_APPROX,
    AMD,
};

// Options for the model render UI. The defaults here are used at startup.
struct ModelRenderOptions {
    // Model.
    glm::quat modelRotation = glm::identity<glm::quat>();
    float modelScale = 1.0f;

    // Rendering.
    LightingModel lightingModel = LightingModel::COOK_TORRANCE_GGX;

    glm::vec3 directionalDiffuse = glm::vec3(0.5f);
    glm::vec3 directionalSpecular = glm::vec3(0.5f);
    float directionalIntensity = 10.0f;
    glm::vec3 directionalDirection = glm::normalize(glm::vec3(-0.2f, -1.0f, -0.3f));

    bool shadowMapping = true;
    float shadowCameraCuboidExtents = 2.0f;
    float shadowCameraNear = 0.1f;
    float shadowCameraFar = 15.0f;
    float shadowCameraDistance = 5.0f;
    float shadowBiasMin = 0.0001;
    float shadowBiasMax = 0.001;

    SkyboxImage skyboxImage = SkyboxImage::KLOPPENHEIM;

    bool useIBL = true;
    glm::vec3 ambientColor = glm::vec3(0.1f);
    bool ssao = true;
    float ssaoRadius = 0.5f;
    float ssaoBias = 0.025f;
    float shininess = 32.0f;
    float emissionIntensity = 5.0f;
    glm::vec3 emissionAttenuation = glm::vec3(0, 0, 1.0f);

    bool bloom = true;
    float bloomMix = 0.004;
    ToneMapping toneMapping = ToneMapping::ACES_APPROX;
    bool gammaCorrect = true;
    float gamma = 2.2f;

    bool fxaa = true;

    // Camera.
    CameraControlType cameraControlType = CameraControlType::ORBIT;
    float speed = 0;
    float sensitivity = 0;
    float fov = 0;
    float nearPlane = 0;
    float farPlane = 0;
    bool captureMouse = false;

    // Debug.
    GBufferVis gBufferVis = GBufferVis::DISABLED;
    bool wireframe = false;
    bool drawNormals = false;

    // Performance.
    const float* frameDeltas = nullptr;
    int numFrameDeltas = 0;
    int frameDeltasOffset = 0;
    float avgFPS = 0;
    bool enableVsync = true;
};

// Helper to display a little (?) mark which shows a tooltip when hovered.
static void imguiHelpMarker(const char* desc) {
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort)) {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

enum class Scale {
    LINEAR = 0,
    LOG,
};

// Helper for a float slider value.
static bool imguiFloatSlider(const char* desc, float* value, float min, float max, const char* fmt = nullptr,
                             Scale scale = Scale::LINEAR) {
    ImGuiSliderFlags flags = ImGuiSliderFlags_None;
    if (scale == Scale::LOG) {
        flags = ImGuiSliderFlags_Logarithmic;
    }
    return ImGui::SliderScalar(desc, ImGuiDataType_Float, value, &min, &max, fmt, flags);
}

// Helper for an image control.
static void imguiImage(const Texture& texture, glm::vec2 size) {
    const auto texID = reinterpret_cast<ImTextureID>(texture.getId());
    // Flip the image.
    ImGui::Image(texID, size, /*uv0=*/glm::vec2(0.0f, 1.0f),
                 /*uv1=*/glm::vec2(1.0f, 0.0f));
}

// Non-normative context for UI rendering. Used for accessing renderer info.
struct UIContext {
    Camera& camera;
    ShadowMap& shadowMap;
    SsaoBuffer& ssaoBuffer;
};

// Called during game loop.
inline void renderImGuiUI(ModelRenderOptions& opts, UIContext ctx) {
    // ImGui::ShowDemoWindow();

    ImGui::Begin("Model Render");

    constexpr float IMAGE_BASE_SIZE = 160.0f;

    if (ImGui::CollapsingHeader("Model", ImGuiTreeNodeFlags_DefaultOpen)) {

        glm::quat rotViewSpace = glm::quat_cast(ctx.camera.getViewTransform()) * opts.modelRotation;
        ImGui::gizmo3D("Model rotation", rotViewSpace, IMAGE_BASE_SIZE);
        opts.modelRotation = glm::quat_cast(glm::inverse(ctx.camera.getViewTransform())) * glm::normalize(rotViewSpace);

        ImGui::SameLine();

        glm::vec3 dirViewSpace = glm::vec3(ctx.camera.getViewTransform() * glm::vec4(opts.directionalDirection, 0.0f));
        ImGui::gizmo3D("Light dir", dirViewSpace, IMAGE_BASE_SIZE);
        opts.directionalDirection =
            glm::vec3(glm::inverse(ctx.camera.getViewTransform()) * glm::vec4(glm::normalize(dirViewSpace), 0.0f));
        ImGui::SliderFloat3("Light dir", reinterpret_cast<float*>(&opts.directionalDirection), -1.0f, 1.0f);

        if (ImGui::Button("Reset rotation")) {
            opts.modelRotation = glm::identity<glm::quat>();
        }
        imguiFloatSlider("Model scale", &opts.modelScale, 0.0001f, 100.0f, "%.04f", Scale::LOG);
    }

    ImGui::Separator();

    // Create a child so that this section can scroll separately.
    ImGui::BeginChild("MainOptions");

    if (ImGui::CollapsingHeader("Rendering")) {
        ImGui::Combo("Lighting model", reinterpret_cast<int*>(&opts.lightingModel),
                     "Blinn-Phong\0Cook-Torrance GGX\0\0");
        ImGui::SameLine();
        imguiHelpMarker("Which lighting model to use for shading.");

        ImGui::Separator();
        if (ImGui::TreeNode("Directional light")) {
            static bool lockSpecular = true;
            ImGui::ColorEdit3("Diffuse color", reinterpret_cast<float*>(&opts.directionalDiffuse),
                              ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR);
            ImGui::BeginDisabled(lockSpecular);
            ImGui::ColorEdit3("Specular color", reinterpret_cast<float*>(&opts.directionalSpecular),
                              ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR);
            ImGui::EndDisabled();
            ImGui::Checkbox("Lock specular", &lockSpecular);
            ImGui::SameLine();
            imguiHelpMarker("Whether to lock the specular light color to the diffuse. Usually "
                            "desired for PBR.");
            if (lockSpecular) {
                opts.directionalSpecular = opts.directionalDiffuse;
            }
            imguiFloatSlider("Intensity", &opts.directionalIntensity, 0.0f, 50.0f, nullptr, Scale::LINEAR);

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Emission lights")) {
            imguiFloatSlider("Emission intensity", &opts.emissionIntensity, 0.0f, 1000.0f, nullptr, Scale::LOG);
            ImGui::DragFloat3("Emission attenuation", reinterpret_cast<float*>(&opts.emissionAttenuation),
                              /*v_speed=*/0.01f, 0.0f, 10.0f);
            ImGui::SameLine();
            imguiHelpMarker("Constant, linear, and quadratic attenuation of emission lights.");

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Shadows")) {
            ImGui::Checkbox("Shadow mapping", &opts.shadowMapping);
            ImGui::BeginDisabled(!opts.shadowMapping);
            // Shadow map texture is a square, so extend both width/height by the
            // aspect ratio.
            imguiImage(ctx.shadowMap.getDepthTexture(), glm::vec2(IMAGE_BASE_SIZE * ctx.camera.getAspectRatio(),
                                                                  IMAGE_BASE_SIZE * ctx.camera.getAspectRatio()));
            imguiFloatSlider("Cuboid extents", &opts.shadowCameraCuboidExtents, 0.1f, 50.0f, nullptr, Scale::LOG);

            if (imguiFloatSlider("Near plane", &opts.shadowCameraNear, 0.01, 1000.0, nullptr, Scale::LOG)) {
                if (opts.shadowCameraNear > opts.shadowCameraFar) {
                    opts.shadowCameraFar = opts.shadowCameraNear;
                }
            }
            if (imguiFloatSlider("Far plane", &opts.shadowCameraFar, 0.01, 1000.0, nullptr, Scale::LOG)) {
                if (opts.shadowCameraFar < opts.shadowCameraNear) {
                    opts.shadowCameraNear = opts.shadowCameraFar;
                }
            }
            imguiFloatSlider("Distance from origin", &opts.shadowCameraDistance, 0.01, 100.0f, nullptr, Scale::LOG);
            if (imguiFloatSlider("Bias min", &opts.shadowBiasMin, 0.0001, 1.0, "%.04f", Scale::LOG)) {
                if (opts.shadowBiasMin > opts.shadowBiasMax) {
                    opts.shadowBiasMax = opts.shadowBiasMin;
                }
            }
            if (imguiFloatSlider("Bias max", &opts.shadowBiasMax, 0.0001, 1.0, "%.04f", Scale::LOG)) {
                if (opts.shadowBiasMax < opts.shadowBiasMin) {
                    opts.shadowBiasMin = opts.shadowBiasMax;
                }
            }

            ImGui::EndDisabled();

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Environment")) {
            ImGui::Combo("Skybox image", reinterpret_cast<int*>(&opts.skyboxImage),
                         "Alex's apt\0Frozen waterfall\0Kloppenheim\0Milkyway\0Mon "
                         "Valley\0Ueno shrine\0Winter forest\0");

            ImGui::BeginDisabled(false);
            ImGui::Checkbox("Use IBL", &opts.useIBL);
            ImGui::EndDisabled();

            ImGui::BeginDisabled(opts.lightingModel != LightingModel::BLINN_PHONG && opts.useIBL);
            ImGui::ColorEdit3("Ambient color", reinterpret_cast<float*>(&opts.ambientColor),
                              ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR);
            ImGui::SameLine();
            imguiHelpMarker("The color of the fixed ambient component.");
            ImGui::EndDisabled();

            ImGui::Checkbox("SSAO", &opts.ssao);
            ImGui::BeginDisabled(!opts.ssao);
            imguiImage(ctx.ssaoBuffer.getSsaoTexture(),
                       glm::vec2(IMAGE_BASE_SIZE * ctx.camera.getAspectRatio(), IMAGE_BASE_SIZE));

            imguiFloatSlider("SSAO radius", &opts.ssaoRadius, 0.01, 5.0, "%.04f", Scale::LOG);
            imguiFloatSlider("SSAO bias", &opts.ssaoBias, 0.0001, 1.0, "%.04f", Scale::LOG);
            ImGui::EndDisabled();

            ImGui::BeginDisabled(opts.lightingModel != LightingModel::BLINN_PHONG);
            imguiFloatSlider("Shininess", &opts.shininess, 1.0f, 1000.0f, nullptr, Scale::LOG);
            ImGui::SameLine();
            imguiHelpMarker("Shininess of specular highlights. Only applies to Phong.");
            ImGui::EndDisabled();

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Post-processing")) {
            ImGui::Checkbox("Bloom", &opts.bloom);
            ImGui::BeginDisabled(!opts.bloom);
            imguiFloatSlider("Bloom mix", &opts.bloomMix, 0.001f, 1.0f, nullptr, Scale::LOG);
            ImGui::EndDisabled();

            ImGui::Combo("Tone mapping", reinterpret_cast<int*>(&opts.toneMapping),
                         "None\0Reinhard\0Reinhard luminance\0ACES (approx)\0AMD\0\0");
            ImGui::Checkbox("Gamma correct", &opts.gammaCorrect);
            ImGui::BeginDisabled(!opts.gammaCorrect);
            imguiFloatSlider("Gamma", &opts.gamma, 0.01f, 8.0f, nullptr, Scale::LOG);
            ImGui::EndDisabled();

            ImGui::Checkbox("FXAA", &opts.fxaa);

            ImGui::TreePop();
        }
    }

    if (ImGui::CollapsingHeader("Camera")) {
        ImGui::RadioButton("Fly controls", reinterpret_cast<int*>(&opts.cameraControlType),
                           static_cast<int>(CameraControlType::FLY));
        ImGui::SameLine();
        ImGui::RadioButton("Orbit controls", reinterpret_cast<int*>(&opts.cameraControlType),
                           static_cast<int>(CameraControlType::ORBIT));

        imguiFloatSlider("Speed", &opts.speed, 0.1, 50.0);
        imguiFloatSlider("Sensitivity", &opts.sensitivity, 0.01, 1.0, nullptr, Scale::LOG);
        imguiFloatSlider("FoV", &opts.fov, MIN_FOV, MAX_FOV, "%.1f°");
        if (imguiFloatSlider("Near plane", &opts.nearPlane, 0.01, 1000.0, nullptr, Scale::LOG)) {
            if (opts.nearPlane > opts.farPlane) {
                opts.farPlane = opts.nearPlane;
            }
        }
        if (imguiFloatSlider("Far plane", &opts.farPlane, 0.01, 1000.0, nullptr, Scale::LOG)) {
            if (opts.farPlane < opts.nearPlane) {
                opts.nearPlane = opts.farPlane;
            }
        }

        ImGui::Checkbox("Capture mouse", &opts.captureMouse);
    }

    if (ImGui::CollapsingHeader("Debug")) {
        ImGui::Combo("G-Buffer vis", reinterpret_cast<int*>(&opts.gBufferVis),
                     "Disabled\0Positions\0Ambient "
                     "occlusion\0Normals\0Roughness\0Albedo\0Metallic\0Emission\0\0");
        ImGui::SameLine();
        imguiHelpMarker("What component of the G-Buffer to visualize.");

        ImGui::Checkbox("Wireframe", &opts.wireframe);
        ImGui::Checkbox("Draw vertex normals", &opts.drawNormals);
    }

    if (ImGui::CollapsingHeader("Performance")) {
        char overlay[32];
        sprintf(overlay, "Avg FPS %.02f", opts.avgFPS);
        ImGui::PlotLines("Frame time", opts.frameDeltas, opts.numFrameDeltas, opts.frameDeltasOffset, overlay, 0.0f,
                         0.03f, ImVec2(0, 80.0f));

        ImGui::Checkbox("Enable VSync", &opts.enableVsync);
    }

    ImGui::EndChild();

    ImGui::End();
    ImGui::Render();
}

/** Loads a model based on command line flag, or a default. */
inline std::unique_ptr<Model> loadModelOrDefault() {
    // Default to the gltf DamagedHelmet.
    auto helmet = std::make_unique<Model>("content/models/Spheres/spheres.gltf");
    return helmet;
}

/** Loads a skybox image as a cubemap and generates IBL info. */
inline void loadSkyboxImage(SkyboxImage skyboxImage, SkyboxMesh& skybox,
                            EquirectCubemapConverter& equirectCubemapConverter,
                            CubemapIrradianceCalculator& irradianceCalculator,
                            GGXPrefilteredEnvMapCalculator& prefilteredEnvMapCalculator) {
    std::string hdrPath;
    switch (skyboxImage) {
    case SkyboxImage::ALEXS_APT:
        hdrPath = "content/skyboxes/AlexsApt/AlexsApt.hdr";
        break;
    case SkyboxImage::FROZEN_WATERFALL:
        hdrPath = "content/skyboxes/FrozenWaterfall/FrozenWaterfall.hdr";
        break;
    case SkyboxImage::KLOPPENHEIM:
        hdrPath = "content/skyboxes/Kloppenheim/Kloppenheim.hdr";
        break;
    case SkyboxImage::MILKYWAY:
        hdrPath = "content/skyboxes/Milkyway/Milkyway.hdr";
        break;
    case SkyboxImage::MON_VALLEY:
        hdrPath = "content/skyboxes/MonValley/MonValley.hdr";
        break;
    case SkyboxImage::UENO_SHRINE:
        hdrPath = "content/skyboxes/UenoShrine/UenoShrine.hdr";
        break;
    case SkyboxImage::WINTER_FOREST:
        hdrPath = "content/skyboxes/WinterForest/WinterForest.hdr";
        break;
    }

    Texture hdr = Texture::loadHdr(hdrPath.c_str());

    // Process HDR cubemap
    {
        DebugGroup debugGroup("HDR equirect to cubemap");
        equirectCubemapConverter.multipassDraw(hdr);
    }
    auto cubemap = equirectCubemapConverter.getCubemap();
    {
        DebugGroup debugGroup("Irradiance calculation");
        irradianceCalculator.multipassDraw(cubemap);
    }
    {
        DebugGroup debugGroup("Prefiltered env map calculation");
        prefilteredEnvMapCalculator.multipassDraw(cubemap);
    }

    skybox.setTexture(cubemap);

    // Don't need this anymore.
    hdr.free();
}
