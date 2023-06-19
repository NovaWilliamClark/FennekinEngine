#include "window.h"

Window::Window(int width, int height, const char* title, bool fullscreen, int samples) {
    Fnk::init();

    if (samples > 0) {
        glfwWindowHint(GLFW_SAMPLES, samples);
    }

    // nullptr indicates windowed.
    GLFWmonitor* monitor = nullptr;
    if (fullscreen) {
        monitor = glfwGetPrimaryMonitor();
    }

    m_window = glfwCreateWindow(width, height, title, monitor, /* share */ nullptr);

    if (m_window == nullptr) {
        Fnk::terminate();
        throw WindowException("ERROR::WINDOW::CREATE_FAILED");
    }

    activate();

    if (glewInit() != GLEW_OK) {
        throw WindowException("ERROR::WINDOW::GLAD_INITIALIZATION_FAILED");
    }

    Fnk::initGlErrorLogging();

    // Allow us to refer to the object while accessing C APIs.
    glfwSetWindowUserPointer(m_window, this);

    // Enable multisampling if needed.
    if (samples > 0) {
        glEnable(GL_MULTISAMPLE);
    }

    // A few options are enabled by default.
    enableDepthTest();
    enableSeamlessCubemap();
    enableResizeUpdates();
    enableKeyInput();
    enableScrollInput();
    enableMouseMoveInput();
    enableMouseButtonInput();
}

Window::~Window() {
    if (m_window != nullptr) {
        glfwDestroyWindow(m_window);
    }
    Fnk::terminate();
}

void Window::activate() {
    glfwMakeContextCurrent(m_window);
}

void Window::updateUniforms(Shader& t_shader) {
    t_shader.setFloat("fnk_deltaTime", m_deltaTime);

    ImageSize size = getSize();
    t_shader.setInt("fnk_windowWidth", size.width);
    t_shader.setInt("fnk_windowHeight", size.height);
}

ImageSize Window::getSize() const {
    ImageSize size;
    glfwGetWindowSize(m_window, &size.width, &size.height);
    return size;
}

void Window::setSize(int t_width, int t_height) {
    glfwSetWindowSize(m_window, t_width, t_height);
}

void Window::enableResizeUpdates() {
    if (m_resizeUpdatesEnabled)
        return;
    auto callback = [](GLFWwindow* window, int width, int height) {
        auto self = static_cast<Window*>(glfwGetWindowUserPointer(window));
        self->framebufferSizeCallback(window, width, height);
    };
    glfwSetFramebufferSizeCallback(m_window, callback);
    m_resizeUpdatesEnabled = true;
}

void Window::disableResizeUpdates() {
    if (!m_resizeUpdatesEnabled)
        return;
    glfwSetFramebufferSizeCallback(m_window, nullptr);
    m_resizeUpdatesEnabled = false;
}

float Window::getAspectRatio() const {
    ImageSize size = getSize();
    return size.width / static_cast<float>(size.height);
}

void Window::enableKeyInput() {
    if (m_keyInputEnabled)
        return;
    auto callback = [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        auto self = static_cast<Window*>(glfwGetWindowUserPointer(window));
        self->keyCallback(key, scancode, action, mods);
    };
    glfwSetKeyCallback(m_window, callback);
    m_keyInputEnabled = true;
}

void Window::disableKeyInput() {
    if (!m_keyInputEnabled)
        return;
    glfwSetKeyCallback(m_window, nullptr);
    m_keyInputEnabled = false;
}

void Window::enableScrollInput() {
    if (m_scrollInputEnabled)
        return;
    auto callback = [](GLFWwindow* window, double xoffset, double yoffset) {
        auto self = static_cast<Window*>(glfwGetWindowUserPointer(window));
        self->scrollCallback(xoffset, yoffset);
    };
    glfwSetScrollCallback(m_window, callback);
    m_scrollInputEnabled = true;
}

void Window::disableScrollInput() {
    if (!m_scrollInputEnabled)
        return;
    glfwSetScrollCallback(m_window, nullptr);
    m_scrollInputEnabled = false;
}

void Window::enableMouseMoveInput() {
    if (m_mouseMoveInputEnabled)
        return;
    auto callback = [](GLFWwindow* window, double xpos, double ypos) {
        auto self = static_cast<Window*>(glfwGetWindowUserPointer(window));
        self->mouseMoveCallback(xpos, ypos);
    };
    glfwSetCursorPosCallback(m_window, callback);
    m_mouseMoveInputEnabled = true;
}

void Window::disableMouseMoveInput() {
    if (!m_mouseMoveInputEnabled)
        return;
    glfwSetCursorPosCallback(m_window, nullptr);
    m_mouseMoveInputEnabled = false;
}

void Window::enableMouseButtonInput() {
    if (m_mouseButtonInputEnabled)
        return;
    auto callback = [](GLFWwindow* window, int button, int action, int mods) {
        auto self = static_cast<Window*>(glfwGetWindowUserPointer(window));
        self->mouseButtonCallback(button, action, mods);
    };
    glfwSetMouseButtonCallback(m_window, callback);
    m_mouseButtonInputEnabled = true;
}

void Window::disableMouseButtonInput() {
    if (!m_mouseButtonInputEnabled)
        return;
    glfwSetMouseButtonCallback(m_window, nullptr);
    m_mouseButtonInputEnabled = false;
}

void Window::framebufferSizeCallback(GLFWwindow* t_window, int t_width, int t_height) {
    glViewport(0, 0, t_width, t_height);

    if (m_boundCamera) {
        m_boundCamera->setAspectRatio(t_width / static_cast<float>(t_height));
    }
    if (m_boundCameraControls) {
        m_boundCameraControls->resizeWindow(t_width, t_height);
    }
}

void Window::makeFullscreen() {
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    auto size = getSize();
    glfwSetWindowMonitor(m_window, monitor, /* unused xpos */ 0,
                         /* unused ypos */ 0, size.width, size.height,
                         /* refreshRate */ GLFW_DONT_CARE);
}

void Window::makeWindowed() {
    auto size = getSize();
    glfwSetWindowMonitor(m_window, /* monitor */ nullptr, /* xpos */ 0,
                         /* ypos */ 0, size.width, size.height,
                         /* refreshRate */ GLFW_DONT_CARE);
}

void Window::processInput(float t_deltaTime) {
    if (m_boundCameraControls) {
        m_boundCameraControls->processInput(m_window, *m_boundCamera, t_deltaTime);
    }
}

void Window::keyCallback(int t_key, int t_scanCode, int t_action, int t_mods) {
    if (m_keyInputPaused)
        return;

    if (t_key == GLFW_KEY_ESCAPE && t_action == GLFW_PRESS) {
        if (m_escBehavior == EscBehavior::TOGGLE_MOUSE_CAPTURE) {
            auto inputMode = glfwGetInputMode(m_window, GLFW_CURSOR);
            if (inputMode == GLFW_CURSOR_NORMAL) {
                enableMouseCapture();
            } else {
                disableMouseCapture();
            }
        } else if (m_escBehavior == EscBehavior::CLOSE) {
            glfwSetWindowShouldClose(m_window, true);
        } else if (m_escBehavior == EscBehavior::UNCAPTURE_MOUSE_OR_CLOSE) {
            auto inputMode = glfwGetInputMode(m_window, GLFW_CURSOR);
            if (inputMode == GLFW_CURSOR_DISABLED) {
                disableMouseCapture();
            } else {
                // Close since mouse is not captured.
                glfwSetWindowShouldClose(m_window, true);
            }
        }
    }

    // Run handlers.
    if (t_action == GLFW_PRESS) {
        for (auto pair : m_keyPressHandlers) {
            int glfwKey;
            std::function<void(int)> handler;
            std::tie(glfwKey, handler) = pair;

            if (t_key == glfwKey) {
                handler(t_mods);
            }
        }
    }
}

void Window::scrollCallback(double t_xOffset, double t_yOffset) {
    if (m_mouseInputPaused)
        return;

    if (m_boundCameraControls) {
        m_boundCameraControls->scroll(*m_boundCamera, t_xOffset, t_yOffset, m_mouseCaptured);
    }
}

void Window::mouseMoveCallback(double t_xPos, double t_yPos) {
    if (m_mouseInputPaused)
        return;

    if (m_boundCameraControls) {
        m_boundCameraControls->mouseMove(*m_boundCamera, t_xPos, t_yPos, m_mouseCaptured);
    }
}

void Window::mouseButtonCallback(int t_button, int t_action, int t_mods) {
    if (m_mouseInputPaused)
        return;

    if (t_button == GLFW_MOUSE_BUTTON_LEFT && t_action == GLFW_PRESS) {
        if (m_mouseButtonBehavior == MouseButtonBehavior::CAPTURE_MOUSE) {
            enableMouseCapture();
        }
    }

    // Run handlers.
    if (t_action == GLFW_PRESS) {
        for (auto pair : m_mouseButtonHandlers) {
            int glfwMouseButton;
            std::function<void(int)> handler;
            std::tie(glfwMouseButton, handler) = pair;

            if (t_button == glfwMouseButton) {
                handler(t_mods);
            }
        }
    }

    if (m_boundCameraControls) {
        m_boundCameraControls->mouseButton(*m_boundCamera, t_button, t_action, t_mods, m_mouseCaptured);
    }
}

void Window::addKeyPressHandler(int t_glfwKey, std::function<void(int)> t_handler) {
    m_keyPressHandlers.push_back(std::make_tuple(t_glfwKey, t_handler));
}
void Window::addMouseButtonHandler(int t_glfwMouseButton, std::function<void(int)> t_handler) {
    m_mouseButtonHandlers.push_back(std::make_tuple(t_glfwMouseButton, t_handler));
}

void Window::enableMouseCapture() {
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    m_mouseCaptured = true;
}

void Window::disableMouseCapture() {
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    m_mouseCaptured = false;
}

void Window::bindCamera(std::shared_ptr<Camera> t_camera) {
    m_boundCamera = t_camera;
    m_boundCamera->setAspectRatio(getSize());
}

void Window::bindCameraControls(std::shared_ptr<CameraControls> t_cameraControls) {
    if (!m_boundCamera) {
        throw WindowException("ERROR::WINDOW::BIND_CAMERA_CONTROLS_FAILED\n"
                              "Camera must be bound before camera controls.");
    }
    m_boundCameraControls = t_cameraControls;
    ImageSize size = getSize();
    m_boundCameraControls->resizeWindow(size.width, size.height);
}

const float* Window::getFrameDeltas() const {
    return &m_frameDeltas[0];
}

int Window::getNumFrameDeltas() const {
    return NUM_FRAME_DELTAS;
}

int Window::getFrameDeltasOffset() const {
    return m_frameCount % NUM_FRAME_DELTAS;
}

float Window::getAvgFps() const {
    int denominator = std::min<int>(m_frameCount, NUM_FRAME_DELTAS);
    float avgFrameDelta = m_frameDeltaSum / denominator;
    return 1.0f / avgFrameDelta;
}

void Window::updateFrameStats(float t_deltaTime) {
    unsigned int offset = getFrameDeltasOffset();
    float oldDeltaTime = m_frameDeltas[offset];
    m_frameDeltaSum -= oldDeltaTime;
    m_frameDeltaSum += t_deltaTime;
    m_frameDeltas[offset] = t_deltaTime;
}

void Window::loop(std::function<void(float)> t_callback) {
    // TODO: Add exception handling here.
    while (!glfwWindowShouldClose(m_window)) {
        float currentTime = Fnk::time();
        m_deltaTime = currentTime - m_lastTime;
        m_lastTime = currentTime;

        updateFrameStats(m_deltaTime);

        // Clear the appropriate buffers.
        glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);
        auto clearBits = GL_COLOR_BUFFER_BIT;
        if (m_depthTestEnabled) {
            clearBits |= GL_DEPTH_BUFFER_BIT;
        }
        if (m_stencilTestEnabled) {
            clearBits |= GL_STENCIL_BUFFER_BIT;
        }
        glClear(clearBits);

        // Process necessary input.
        processInput(m_deltaTime);

        // Call the loop function.
        t_callback(m_deltaTime);

        FNK_CHECK_FOR_GL_ERROR();

        glfwSwapBuffers(m_window);
        glfwPollEvents();

        ++m_frameCount;
    }
}
