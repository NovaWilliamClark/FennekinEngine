#pragma once
#include <string>
#include <filesystem>

#include "camera.hpp"
#include <nlohmann/json.hpp>
class Scene
{
public:
    explicit Scene(const std::string& t_sceneID, bool& t_status);
    ~Scene();

    void update(float_t t_deltaTime);

    // TODO: Forward rendering drawing functions. 
    //The reason these are called here and not inside the renderer class is that most of the data 
    //is located within this class. There might be an opportunity to restructure this in a more data oriented 
    //way, but I don't see how just yet. Possibly just keeping a struct of arrays of all the items in the scene?

    //void drawPointLightShadow(const Shader& pointLightShader, unsigned int index, unsigned int cubeMapTarget);
    //void drawDirLightShadows(const Shader& dirLightShader, unsigned int targetTextureID);
    //void drawFullScene(const Shader& mainSceneShader, const Shader& skyboxShader);
    //void drawDepthPass(const Shader& depthPassShader);

    // TODO: Create the render queue
    Camera* getCurrentCamera();
    int8_t getShadowResolution();

private:
    std::string m_sceneID;
    bool m_shouldDisplayClusterSlices;

    bool loadSceneContent();
    bool loadSkyBox(nlohmann::json& t_sceneData);
    bool loadLights(nlohmann::json& t_sceneData);
    bool loadCamera(nlohmann::json& t_sceneData);
    bool loadModels(nlohmann::json& t_sceneData);
};

