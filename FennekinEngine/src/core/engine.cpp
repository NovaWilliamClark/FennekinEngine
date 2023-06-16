#include "core/engine.hpp"

#include <iostream>

#include "logger.hpp"
#include "core/time.hpp"
#include "core/window.hpp"

Engine::Engine(): m_window(false) {
	
}

Engine::~Engine() = default;

void Engine::run() {
	float_t deltaTime = m_timer.getDeltaTime();

	LOG_INFO("Engine Initialization took: {0}ms", m_timer.endTimer<Time::Milliseconds>());
	LOG_INFO("Entering the main loops");

	while (!m_window.shouldClose()) {
		m_timer.update();
		m_window.clear();

		m_window.swapBuffers();
	}

}