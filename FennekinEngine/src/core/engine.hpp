#pragma once
#include "time.hpp"
#include "Window.hpp"

class Engine
{
public:
    Engine();
    ~Engine();

    void run();

private:
    Window m_window;
    Time m_timer;
};

