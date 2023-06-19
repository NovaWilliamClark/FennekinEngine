#pragma once

// clang-format off
// Must precede glfw/glad, to include OpenGL functions.
#include "core.h"
// clang-format on

#include "core/debug/exceptions.h"
#include "rendering/interfaces/screen.h"
#include "rendering/resources/shader.h"
#include "scene/camera.h"
#include <GLFW/glfw3.h>
#include <gl/glew.h>

#include <functional>
#include <glm/glm.hpp>
#include <memory>
#include <tuple>
#include <vector>

class WindowException : public QuarkException {
    using QuarkException::QuarkException;
};

constexpr int DEFAULT_WIDTH = 800;
constexpr int DEFAULT_HEIGHT = 600;
constexpr char const* DEFAULT_TITLE = "FennikinEngine - William Clark";
constexpr glm::vec4 DEFAULT_CLEAR_COLOR = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

// Controls special convenience behavior when Esc is pressed.
enum class EscBehavior {
    NONE,
    TOGGLE_MOUSE_CAPTURE,
    CLOSE,
    // Uncaptures the mouse if it is captured, or else closes the app.
    UNCAPTURE_MOUSE_OR_CLOSE,
};

// Controls special convenience behavior when the LMB is pressed.
enum class MouseButtonBehavior {
    NONE,
    CAPTURE_MOUSE,
};

class Window : public UniformSource {
public:
    Window(int width = DEFAULT_WIDTH, int height = DEFAULT_HEIGHT, const char* title = DEFAULT_TITLE,
           bool fullscreen = false, int samples = 0);
    virtual ~Window();
    GLFWwindow* getGlfwRef() const {
        return m_window;
    }

    // TODO: Should this be called something different, and 'activate' be used for
    // setViewport?
    void activate();

    void setViewport() {
        ImageSize size = getSize();
        glViewport(0, 0, size.width, size.height);
    }

    void enableVsync() {
        activate();
        glfwSwapInterval(1);
    }
    void disableVsync() {
        activate();
        glfwSwapInterval(0);
    }

    // TODO: Extract all these as a "Context" object.
    void enableDepthTest() {
        glEnable(GL_DEPTH_TEST);
        m_depthTestEnabled = true;
    }
    void disableDepthTest() {
        glDisable(GL_DEPTH_TEST);
        m_depthTestEnabled = false;
    }

    // TODO: Consider extracting stencil logic out to a separate class.
    void enableStencilTest() {
        glEnable(GL_STENCIL_TEST);
        // Only replace the value in the stencil buffer if both the stencil and
        // depth test pass.
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        m_stencilTestEnabled = true;
    }
    void disableStencilTest() {
        glDisable(GL_STENCIL_TEST);
        m_stencilTestEnabled = false;
    }

    void enableStencilUpdates() {
        glStencilMask(0xFF);
    }
    void disableStencilUpdates() {
        glStencilMask(0x00);
    }

    void stencilAlwaysDraw() {
        setStencilFunc(GL_ALWAYS);
    }
    void stencilDrawWhenMatching() {
        setStencilFunc(GL_EQUAL);
    }
    void stencilDrawWhenNotMatching() {
        setStencilFunc(GL_NOTEQUAL);
    }
    void setStencilFunc(GLenum func) {
        // Set the stencil test to use the given `func` when comparing for fragment
        // liveness.
        glStencilFunc(func, /*ref=*/1, /*mask=*/0xFF);
    }

    // TODO: Consider extracting blending logic.
    void enableAlphaBlending() {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBlendEquation(GL_FUNC_ADD);
    }
    void disableAlphaBlending() {
        glDisable(GL_BLEND);
    }

    void enableFaceCull() {
        glEnable(GL_CULL_FACE);
    }
    void disableFaceCull() {
        glDisable(GL_CULL_FACE);
    }

    void enableWireframe() {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    void disableWireframe() {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    void enableSeamlessCubemap() {
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    }
    void disableSeamlessCubemap() {
        glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    }

    void cullFrontFaces() {
        glCullFace(GL_FRONT);
    }
    void cullBackFaces() {
        glCullFace(GL_BACK);
    }

    void updateUniforms(Shader& t_shader) override;

    [[nodiscard]] ImageSize getSize() const;
    void setSize(int t_width, int t_height);
    void enableResizeUpdates();
    void disableResizeUpdates();
    [[nodiscard]] float getAspectRatio() const;

    [[nodiscard]] unsigned int getFrameCount() const {
        return m_frameCount;
    }

    [[nodiscard]] const float* getFrameDeltas() const;
    [[nodiscard]] int getNumFrameDeltas() const;
    [[nodiscard]] int getFrameDeltasOffset() const;
    [[nodiscard]] float getAvgFps() const;

    [[nodiscard]] glm::vec4 getClearColor() const {
        return m_clearColor;
    }
    void setClearColor(const glm::vec4 t_color) {
        m_clearColor = t_color;
    }

    void makeFullscreen();
    void makeWindowed();

    [[nodiscard]] EscBehavior getEscBehavior() const {
        return m_escBehavior;
    }
    void setEscBehavior(const EscBehavior t_behavior) {
        m_escBehavior = t_behavior;
    }

    [[nodiscard]] MouseButtonBehavior getMouseButtonBehavior() const {
        return m_mouseButtonBehavior;
    }
    void setMouseButtonBehavior(const MouseButtonBehavior t_behavior) {
        m_mouseButtonBehavior = t_behavior;
    }

    void enableKeyInput();
    void disableKeyInput();
    void enableScrollInput();
    void disableScrollInput();
    void enableMouseMoveInput();
    void disableMouseMoveInput();
    void enableMouseButtonInput();
    void disableMouseButtonInput();

    void setKeyInputPaused(const bool t_isPaused) {
        m_keyInputPaused = t_isPaused;
    }
    void setMouseInputPaused(const bool t_isPaused) {
        m_mouseInputPaused = t_isPaused;
    }

    void enableMouseCapture();
    void disableMouseCapture();

    void bindCamera(std::shared_ptr<Camera> t_camera);
    void bindCameraControls(std::shared_ptr<CameraControls> t_cameraControls);

    // TODO: Add a way to remove handlers.
    void addKeyPressHandler(int t_glfwKey, std::function<void(int)> t_handler);
    void addMouseButtonHandler(int t_glfwMouseButton, std::function<void(int)> t_handler);

    void loop(std::function<void(float)> t_callback);

    // TODO: Allow setting window icon.

private:
    void processInput(float t_deltaTime);
    void keyCallback(int t_key, int t_scanCode, int t_action, int t_mods);
    void scrollCallback(double t_xOffset, double t_yOffset);
    void mouseMoveCallback(double t_xPos, double t_yPos);
    void mouseButtonCallback(int t_button, int t_action, int t_mods);
    void framebufferSizeCallback(GLFWwindow* t_window, int t_width, int t_height);

    void updateFrameStats(float t_deltaTime);

    GLFWwindow* m_window;
    bool m_depthTestEnabled = false;
    bool m_stencilTestEnabled = false;

    float m_lastTime = 0.0f;
    float m_deltaTime = 0.0f;
    unsigned int m_frameCount = 0;
    static constexpr int NUM_FRAME_DELTAS = 120;
    float m_frameDeltas[NUM_FRAME_DELTAS] = {0.0f};
    float m_frameDeltaSum = 0.0f;
    glm::vec4 m_clearColor = DEFAULT_CLEAR_COLOR;

    EscBehavior m_escBehavior = EscBehavior::NONE;
    MouseButtonBehavior m_mouseButtonBehavior = MouseButtonBehavior::NONE;
    bool m_resizeUpdatesEnabled = false;
    bool m_keyInputEnabled = false;
    bool m_scrollInputEnabled = false;
    bool m_mouseMoveInputEnabled = false;
    bool m_mouseButtonInputEnabled = false;
    bool m_keyInputPaused = false;
    bool m_mouseInputPaused = false;
    bool m_mouseCaptured = false;
    std::vector<std::tuple<int, std::function<void(int)>>> m_keyPressHandlers;
    std::vector<std::tuple<int, std::function<void(int)>>> m_mouseButtonHandlers;

    std::shared_ptr<Camera> m_boundCamera = nullptr;
    std::shared_ptr<CameraControls> m_boundCameraControls = nullptr;
};
