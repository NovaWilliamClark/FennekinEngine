#pragma once
#include "core/time.hpp"
#include "core/window.hpp"
#include "scene/sceneManager.hpp"


class Engine
{
public:
    Engine();
    ~Engine();

    void run();

private:
    Time m_timer;
    Window m_window;
    Camera m_camera;
    SceneManager m_sceneManager;
};

