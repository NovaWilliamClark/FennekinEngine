#include "io/inputManager.hpp"
#include "core/window.hpp"
#include <iostream>
#include <glm/glm.hpp>

InputManager::InputManager(Window* t_window) {
    m_window = t_window;

    // Set this input manager as the window's user pointer
    glfwSetWindowUserPointer(m_window->getNativeWindow(), this);

    // Set the callback functions
    glfwSetCursorPosCallback(m_window->getNativeWindow(), cursorPositionCallback);
    glfwSetScrollCallback(m_window->getNativeWindow(), scrollCallback);
}

void InputManager::mapKeyToEventAction(const int32_t t_key, const Action& t_action) {
    m_eventActions[t_key] = t_action;
}

void InputManager::mapKeyToContinuousAction(const int32_t t_key, const Action& t_action) {
    m_continuousActions[t_key] = t_action;
}

void InputManager::remapKey(const int32_t t_oldKey, const int32_t t_newKey) {
    if (m_eventActions.contains(t_oldKey)) {
        m_eventActions[t_newKey] = m_eventActions[t_oldKey];
        m_eventActions.erase(t_oldKey);
    }

    // Check for continuous actions.
    if (m_continuousActions.contains(t_oldKey)) {
        m_continuousActions[t_newKey] = m_continuousActions[t_oldKey];
        m_continuousActions.erase(t_oldKey);
    }

    // If the old key is currently pressed, make sure the new key is also.
    if (m_pressedKeys.contains(t_oldKey)) {
        m_pressedKeys.erase(t_oldKey);
        m_pressedKeys.insert(t_newKey);
    }
}

void InputManager::update() {
    glfwPollEvents();
    processKey();
}

void InputManager::processKey() {
    // Process single press actions.
    for (const auto& [key, action] : m_eventActions) {
        if (glfwGetKey(m_window->getNativeWindow(), key) == GLFW_PRESS) {
            if (!m_pressedKeys.contains(key)) {
                action();
                m_pressedKeys.insert(key);
            }
        }
        else {
            m_pressedKeys.erase(key);
        }
    }

    // Process continuous actions.
	for (const auto& [key, action] : m_continuousActions) {
        const int keyState = glfwGetKey(m_window->getNativeWindow(), key);
		if (keyState == GLFW_PRESS || keyState == GLFW_REPEAT) {
			action();
		}
	}
}

void InputManager::cursorPositionCallback(GLFWwindow* t_window, const double t_xPos, const double t_yPos) {
    // Call the registered mouse move action
    const auto* manager = static_cast<InputManager*>(glfwGetWindowUserPointer(t_window));
    if (manager->m_mouseMoveAction) {
        manager->m_mouseMoveAction(t_xPos, t_yPos);
    }
}

void InputManager::scrollCallback(GLFWwindow* t_window, const double t_xOffset, const double t_yOffset) {
    // Call the registered scroll action
    const auto* manager = static_cast<InputManager*>(glfwGetWindowUserPointer(t_window));
    if (manager->m_scrollAction) {
        manager->m_scrollAction(t_xOffset, t_yOffset);
    }
}
