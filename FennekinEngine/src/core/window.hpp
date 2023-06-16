#pragma once
#include <glm/vec4.hpp>

#include "renderer/core/context.hpp"
#include "renderer/interfaces/UniformSource.hpp"
#include "renderer/resources/shader.hpp"

struct ImageSize {
	int width;
	int height;

	inline bool operator==(const ImageSize& t_comparison) const {
		return this->width == t_comparison.width && this->height == t_comparison.height;
	}
	inline bool operator!=(const ImageSize& t_comparison) const {
		return !operator==(t_comparison);
	}
};

struct GLFWwindow;
typedef unsigned int GLenum;

class Window final : public UniformSource
{
public:
	Window() = default;
	explicit Window(bool t_fullscreen);
	~Window() override;

	void setViewport();
	void enableVSync();
	void disableVSync();
	void updateUniforms(Shader& t_shader, float_t t_deltaTime) override;


	ImageSize getSize() const;
	void setSize(int t_width, int t_height);
	void enableResizeUpdates();
	void disableResizeUpdates();
	float_t getAspectRatio() const;

	void clear();

	void swapBuffers() const;
	bool shouldClose() const;
	GLFWwindow* getNativeWindow() const;

private:

	bool initGLFW();
	bool initGLEW(GLenum& t_errorCode);
	bool createWindow(bool t_fullscreen);
	bool createContext();

	static void errorCallback(int t_error, const char* t_description);

	GLFWwindow* m_window = nullptr;
	Context m_context;
};