#pragma once

// OpenGL included by glad, so disable GLFW's attempts to load extensions.
#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <gl/glew.h>

namespace Fnk {
    void init();
    void terminate();
    void enableGlfwErrorLogging();
    void disableGlfwErrorLogging();
    void initGlErrorLogging();
    void enableGlErrorLogging();
    void disableGlErrorLogging();
    float time();
}

