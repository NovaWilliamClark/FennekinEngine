#pragma once
#include <functional>
#include <unordered_set>

#include <glfw/glfw3.h>


class Window;

class InputManager {
public:
    using Action = std::function<void()>;
    using MouseMoveAction = std::function<void(double, double)>;
    using ScrollAction = std::function<void(double, double)>;

    explicit InputManager(Window* t_window);

    void mapKeyToEventAction(int32_t t_key, const Action& t_action);
    void mapKeyToContinuousAction(int32_t t_key, const Action& t_action);

    void remapKey(int32_t t_oldKey, int32_t t_newKey);

    void update();
    void processKey();

    static void cursorPositionCallback(GLFWwindow* t_window, double t_xPos, double t_yPos);

    static void scrollCallback(GLFWwindow* t_window, double t_xOffset, double t_yOffset);

    void setMouseMoveAction(const MouseMoveAction& t_action) {
        m_mouseMoveAction = t_action;
    }

    void setScrollAction(const ScrollAction& t_action) {
        m_scrollAction = t_action;
    } 

private:
    Window* m_window;
    std::unordered_map<int32_t, Action> m_eventActions;
    std::unordered_map<int32_t, Action> m_continuousActions;
    std::unordered_set<int32_t> m_pressedKeys;
    MouseMoveAction m_mouseMoveAction;
    ScrollAction m_scrollAction;
};
