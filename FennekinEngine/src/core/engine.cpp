#include "core/engine.hpp"

#include <iostream>

#include "logger.hpp"
#include "core/time.hpp"
#include "core/window.hpp"

Engine::Engine()
{
	m_timer = Time();
	m_window = Window();
	m_inputManager = InputManager(&m_window);
}

Engine::~Engine() = default;

void Engine::run() {
	LOG_INFO("Engine Initialization took: {0}ms", m_timer.endTimer<Time::Milliseconds>());
	float_t deltaTime = m_timer.getDeltaTime();

	// Map the 'A' key to a single press action that prints a message
	m_inputManager.mapKeyToEventAction(GLFW_KEY_A, []() {
		// Implement Object Movement
		LOG_INFO("'B' key is being pressed");
	});

	// Map the 'B' key to a continuous action that prints a message
	m_inputManager.mapKeyToContinuousAction(GLFW_KEY_B, []() {
		// Implement Object Movement
		LOG_INFO("'B' key is being pressed");
	});

	LOG_INFO("Entering the main loops");

	while (!m_window.shouldClose()) {
		m_timer.update();
		m_window.clear();
		m_inputManager.update();

		m_window.swapBuffers();
	}

}