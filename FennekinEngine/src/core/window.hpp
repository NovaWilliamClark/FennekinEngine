#pragma once

struct GLFWwindow;
typedef unsigned int GLenum;

class Window
{
public:
 Window();
 ~Window();


 void clear();

 void swapBuffers() const;
 bool shouldClose() const;
 GLFWwindow* getNativeWindow() const;

 static constexpr int SCREEN_WIDTH = 1280;
 static constexpr int SCREEN_HEIGHT = 720;
 static constexpr float_t SCREEN_ASPECT_RATIO = static_cast<float_t>(SCREEN_WIDTH) / static_cast<float_t>(SCREEN_HEIGHT);

private:

 bool initGLFW();

 bool initGLEW(GLenum& t_errorCode);

 bool createWindow();

 bool createContext() const;

 static void errorCallback(int t_error, const char* t_description);

 GLFWwindow* m_window = nullptr;
};