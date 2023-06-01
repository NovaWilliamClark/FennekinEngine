#pragma once

struct GLFWwindow;
class Window
{
public:
	// Public

	/**
	 * Initializes the GLFW window and the OpenGL context
	 */
	Window();

	/**
	 * Destroys the GLFW window and the OpenGL context
	 */
	~Window();

	/**
	 * Binds the display framebuffer for drawing
	 */
	void bind();

	/*
	 * Swaps the finished draw buffer with the display buffer
	 */
	void swapBuffer();

	static constexpr int SCREEN_WIDTH = 1280; // Screen width in pixels
	static constexpr int SCREEN_HEIGHT = 720; // Screen height in pixels
	static constexpr float SCREEN_ASPECT_RATIO = static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT);
private:
	// Private

	GLFWwindow* m_window = nullptr;

};

