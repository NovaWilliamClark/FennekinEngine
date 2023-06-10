#pragma once
#include "core/time.hpp"
#include "core/window.hpp"
#include "io/inputManager.hpp"
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
    InputManager m_inputManager;
    SceneManager m_sceneManager;
};

