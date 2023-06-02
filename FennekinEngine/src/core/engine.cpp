#include "core/engine.hpp"

#include "logger.hpp"
#include "core/time.hpp"
#include "core/window.hpp"

Engine::Engine() = default;

Engine::~Engine() = default;

void Engine::run() {
    LOG_INFO("Engine Initialization took: {0}ns", m_timer.endTimer<Time::Nanoseconds>());
    uint64_t iteration = 0;
    //uint32_t deltaTime = static_cast<uint32_t>(m_timer.getDeltaTime());

    LOG_INFO("Entering the main loops");

    while (!m_window.shouldClose()) {
        ++iteration;
        m_timer.startTimer();
        m_window.bind();

        LOG_INFO("Frametime: {0}ns", m_timer.endTimer<Time::Nanoseconds>());
        m_timer.update();

        m_window.swapBuffers();
    }
}
