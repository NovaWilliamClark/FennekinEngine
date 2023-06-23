#pragma once
#include "window.hpp"
#include "core/time.hpp"
#include "pch.hpp"


class Engine
{
public:
    Engine();
    ~Engine();

    void run();

private:
    Time m_timer;


    Window m_window{};

};

