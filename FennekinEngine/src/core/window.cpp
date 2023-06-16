#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "core/logger.hpp"
#include "core/Window.hpp"
#include "utilities/Constants.hpp"

Window::Window(const bool t_fullscreen) {
    // Set the error callback function
    glfwSetErrorCallback(errorCallback);

    if (!initGLFW()) {
        LOG_CRITICAL("Failed to initialize GLFW");
    }

    if (!createWindow(t_fullscreen)) {
        LOG_CRITICAL("Failed to create an GLFW window");
    }

    if (!createContext()) {
        LOG_CRITICAL("Failed to create an OpenGL Context");
    }

    GLenum errorCode = 0;

    if (!initGLEW(errorCode)) {
        switch (errorCode) {
        case GLEW_ERROR_NO_GL_VERSION: LOG_CRITICAL(
            "GLEW failed to initialize! OpenGL version could not be determined");
        case GLEW_ERROR_GL_VERSION_10_ONLY: LOG_CRITICAL(
            "GLEW failed to initialize! OpenGL 1.1 or higher is required");
        case GLEW_ERROR_GLX_VERSION_11_ONLY: LOG_CRITICAL(
            "GLEW failed to initialize! GLX 1.2 or higher is required");
        case GLEW_ERROR_NO_GLX_DISPLAY: LOG_CRITICAL(
            "GLEW failed to initialize! GLX display could not be determined");
        default: LOG_CRITICAL("Failed to initialize GLEW, UNKNOWN ERROR");
        }
    }

    glfwSetWindowUserPointer(m_window, this);
}

Window::~Window() {
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void Window::setViewport() {}
void Window::enableVSync() {}
void Window::disableVSync() {}

void Window::updateUniforms(Shader& t_shader, float_t t_deltaTime) {
    //t_shader.setUniform("deltaTime", t_deltaTime);
    //
    //ImageSize size = getSize();
    //t_shader.setUniform("windowWidth", size.width);
    //t_shader.setUniform("windowHeight", size.height);
}

void Window::clear() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::swapBuffers() const {
    glfwSwapBuffers(m_window);
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(m_window);
}

GLFWwindow* Window::getNativeWindow() const {
    return m_window;
}

bool Window::initGLFW() {
    // Initialize GLFW
    if (glfwInit() == GLFW_FALSE) {
        return false;
    }

    // Set GLFW window hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);                 // Set major GL version to 4
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);                 // Set minor GL version to 6
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Set OpenGL profile to core
    glfwWindowHint(GLFW_SRGB_CAPABLE, GL_TRUE);                    // Enable sRGB framebuffer capability
    glfwWindowHint(GLFW_SAMPLES, 4);                               // Level of MSAA if multi sampling is enabled

    return true;
}

bool Window::initGLEW(GLenum& t_errorCode) {
    // Initialize GLEW
    t_errorCode = glewInit();
    if (t_errorCode != GLEW_OK) {
        return false;
    }
    return true;
}

bool Window::createWindow(const bool t_fullscreen) {

    // Get Primary Monitor
    GLFWmonitor* monitor = nullptr;
    if (t_fullscreen) {
        monitor = glfwGetPrimaryMonitor();
    }

    // Create the window
    m_window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Fennekin Engine - William Clark", monitor, nullptr);
    if (!m_window) {
        glfwTerminate();
        return false;
    }
    return true;
}

bool Window::createContext() {
    // Make the window's context current
    glfwMakeContextCurrent(m_window);

    if (!glfwGetCurrentContext()) {
        return false;
    }

    LOG_INFO("GLFW & OpenGL Core Context Successfuly Inititalized\n");

    LOG_INFO("OpenGL Version: {0}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    LOG_INFO("OpenGL Vendor: {0}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
    LOG_INFO("OpenGL Renderer: {0}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));

    m_context.enableDepthTest();
    m_context.enableSeamlessCubemap();
    m_context.enableAlphaBlending();
    m_context.enableStencilTest();
    m_context.enableStencilUpdates();
    m_context.enableMultiSampling();

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); // Set the viewport to the window size
    return true;
}

void Window::errorCallback(int t_error, const char* t_description) {
    LOG_ERROR_NBP("GLFW ERROR: {} {}", t_error, t_description);
}
