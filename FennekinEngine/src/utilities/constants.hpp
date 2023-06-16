#pragma once
#undef DEFAULT_PITCH // resolves windgi.h conflict
#include <glm/vec4.hpp>

// Screen Constants
constexpr int SCREEN_WIDTH = 1280;
constexpr int SCREEN_HEIGHT = 720;
constexpr glm::vec4 DEFAULT_CLEAR_COLOR = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

// Camera Constants
constexpr float_t DEFAULT_YAW = 270.0f;
constexpr float_t DEFAULT_PITCH = 0.1f;
constexpr float_t DEFAULT_FOV = 45.0f;
constexpr float_t DEFAULT_NEAR = 0.1f;
constexpr float_t DEFAULT_FAR = 100.0f;
constexpr float_t DEFAULT_ASPECT_RATIO = 4.0f / 3.0f;
constexpr float_t MIN_FOV = 1.0f;
constexpr float_t MAX_FOV = 135.0f;
constexpr float_t POLAR_CAP = 90.0f - 0.1f;
