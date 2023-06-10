#include "Scene.hpp"

#include <fstream>

#include "core/logger.hpp"
#include <nlohmann/json.hpp>

Scene::Scene(const std::string& t_sceneID, bool& t_status) {
    const std::string filePath = "content/scenes/" + t_sceneID + ".json";
    if (!std::filesystem::exists(filePath)) {
        LOG_WARNING("Cannot Access: {}. Either file does not exist or the given ID is invalid.");
        t_status = false;
    } else {
        m_sceneID = t_sceneID;
        t_status = loadSceneContent();
    }
}

Scene::~Scene() {}
void Scene::update(float_t t_deltaTime) {
}

Camera* Scene::getCurrentCamera() {
    return nullptr;
}

int8_t Scene::getShadowResolution() {
    return 0;
}

bool Scene::loadSceneContent() {
    bool isSuccessful = true;
    const std::string filePath = "content/scenes/" + m_sceneID + ".json";

    nlohmann::json sceneData;
    std::ifstream file(filePath.c_str());       // Load Json file into memory
    file >> sceneData;                                  // Parse all data as as a json file


    loadCamera(sceneData);
    loadSkyBox(sceneData);
    loadLights(sceneData);
    loadModels(sceneData);

    return isSuccessful;
}

bool Scene::loadCamera(nlohmann::json& t_sceneData) {
    LOG_INFO("Loading Scene Camera");
    nlohmann::json cameraSetting = t_sceneData["camera"];
    // TODO: Load in Camera config

    return true;
}

bool Scene::loadSkyBox(nlohmann::json& t_sceneData) {
    LOG_INFO("Loading Scene Skybox");
    nlohmann::json cameraSetting = t_sceneData["camera"];
    // TODO: Load in Camera config

    return true;
}
bool Scene::loadLights(nlohmann::json& t_sceneData) {
    LOG_INFO("Loading Scene Lights");
    nlohmann::json cameraSetting = t_sceneData["camera"];
    // TODO: Load in Camera config

    return true;
}
bool Scene::loadModels(nlohmann::json& t_sceneData) {
    LOG_INFO("Loading Scene Models");
    nlohmann::json cameraSetting = t_sceneData["camera"];
    // TODO: Load in Camera config

    return true;
}
