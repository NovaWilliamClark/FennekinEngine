#pragma once
#include "Scene.hpp"

class SceneManager
{
public:
    SceneManager();
    ~SceneManager();

    void update(float_t t_deltaTime);
    bool switchScene(std::string& t_sceneID);
    Scene* getCurrentScene() const;

private:
    bool loadScene(std::string& t_sceneID);

    std::unique_ptr<Scene> m_scene;
};

