#include "core/engine.hpp"

#include <iostream>

#include "logger.hpp"
#include "core/time.hpp"
#include "core/window.hpp"

Engine::Engine() = default;

Engine::~Engine() = default;

void Engine::run() {
    LOG_INFO("Engine Initialization took: {0}ns", m_timer.endTimer<Time::Nanoseconds>());
    //uint32_t deltaTime = static_cast<uint32_t>(m_timer.getDeltaTime());

    LOG_INFO("Entering the main loops");

    while (!m_window.shouldClose()) {
        m_timer.update();

        m_window.bind();

        m_window.swapBuffers();
    }

}