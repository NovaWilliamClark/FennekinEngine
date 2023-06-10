#include "sceneManager.hpp"

#include "core/logger.hpp"

SceneManager::SceneManager() {
    std::string initialSceneID = "Intel_Sponza";
    if (!loadScene(initialSceneID)) {
        LOG_CRITICAL("Failed to Load {0}. Ensure Scene Data is availible");
    }
}

SceneManager::~SceneManager() {}
void SceneManager::update(float_t t_deltaTime) {}

bool SceneManager::switchScene(std::string& t_sceneID) {
    return false;
}

bool SceneManager::loadScene(std::string& t_sceneID) {
    bool isSuccessful;
    m_scene = std::make_unique<Scene>(t_sceneID, isSuccessful);
    return isSuccessful;
}
