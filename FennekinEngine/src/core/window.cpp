#include "Window.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "core/logger.hpp"

Window::Window() {

	// TODO: Add GLFW error callback

	// Initialize GLFW
	if (glfwInit() == GLFW_FALSE) {
		LOG_CRITICAL("Failed to initialize GLFW");
	}

	// Set GLFW window hints
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);					// Set major GL version to 4
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);					// Set minor GL version to 6
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	// Set OpenGL profile to core
	glfwWindowHint(GLFW_SRGB_CAPABLE, GL_TRUE);					// Enable sRGB framebuffer capability

	// Create the window
	m_window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Fennekin Engine", nullptr, nullptr);
	if (!m_window) {
		glfwTerminate();
		LOG_CRITICAL("Failed to create GLFW window");
	}

	// Make the window's context current
	glfwMakeContextCurrent(m_window);

	// Initialize GLEW
	if (const int errorCode = glewInit(); errorCode != GLEW_OK) {
		switch (errorCode) {
		case GLEW_ERROR_NO_GL_VERSION: LOG_CRITICAL("GLEW failed to initialize! OpenGL version could not be determined");
		case GLEW_ERROR_GL_VERSION_10_ONLY: LOG_CRITICAL("GLEW failed to initialize! OpenGL 1.1 or higher is required");
		case GLEW_ERROR_GLX_VERSION_11_ONLY: LOG_CRITICAL("GLEW failed to initialize! GLX 1.2 or higher is required");
		case GLEW_ERROR_NO_GLX_DISPLAY: LOG_CRITICAL("GLEW failed to initialize! GLX display could not be determined");
		default: LOG_CRITICAL("Failed to initialize GLEW, UNKNOWN ERROR");
		}
	}

	LOG_INFO("GLFW & OpenGL Core Context Successfuly Inititalized\n");

	LOG_INFO("OpenGL Version: {0}", glGetString(GL_VERSION));
	LOG_INFO("OpenGL Vendor: {0}", glGetString(GL_VENDOR));
	LOG_INFO("OpenGL Renderer: {0}", glGetString(GL_RENDERER));

	glfwSwapInterval(0); // Disable VSync
	glEnable(GL_FRAMEBUFFER_SRGB); // Enable sRGB conversion for the default framebuffer
	glEnable(GL_CULL_FACE); // Enable backface culling
	glEnable(GL_MULTISAMPLE); // Enable multisampling
	glEnable(GL_FRAMEBUFFER_SRGB); // Enable sRGB conversion for the default framebuffer
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS); // Enable seamless cubemap sampling

	int width, height;
	glfwGetWindowSize(m_window, &width, &height);
	glViewport(0, 0, width, height); // Set the viewport to the window size
}

Window::~Window() {
    glfwDestroyWindow(m_window);
    glfwTerminate();
}
void Window::bind() {

}
void Window::swapBuffer() {

}
