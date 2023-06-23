#pragma once

// clang-format off
// Must precede glfw/glad, to include OpenGL functions.
#include "core/core.hpp"
// clang-format on

#include "rendering/interfaces/screen.hpp"
#include "rendering/resources/shader.hpp"
#include "scene/lighting/light.hpp"
#include <GLFW/glfw3.h>
#include <gl/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#undef DEFAULT_PITCH

enum class ECameraDirection {
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT,
  UP,
  DOWN,
};

constexpr float DEFAULT_YAW = 270.0f;
constexpr float DEFAULT_PITCH = 0.0f;
constexpr float DEFAULT_FOV = 45.0f;
constexpr float DEFAULT_NEAR = 0.1f;
constexpr float DEFAULT_FAR = 100.0f;
constexpr float DEFAULT_ASPECT_RATIO = 4.0f / 3.0f;

constexpr float MIN_FOV = 1.0f;
constexpr float MAX_FOV = 135.0f;

class Camera final : public UniformSource, public ViewSource {
public:
  // Constructs a new Camera. Angular values should be provided in degrees.
  explicit Camera(glm::vec3 t_position = glm::vec3(0.0f),
                  glm::vec3 t_worldUp = glm::vec3(0.0f, 1.0f, 0.0f),
                  float t_yaw = DEFAULT_YAW, float t_pitch = DEFAULT_PITCH,
                  float t_fov = DEFAULT_FOV,
                  float t_aspectRatio = DEFAULT_ASPECT_RATIO,
                  float t_near = DEFAULT_NEAR, float t_far = DEFAULT_FAR);
  ~Camera() override;

  void lookAt(glm::vec3 t_center);

  [[nodiscard]] glm::vec3 getPosition() const;
  void setPosition(const glm::vec3 t_position);
  [[nodiscard]] float getYaw() const;

  void setYaw(const float t_yaw);
  [[nodiscard]] float getPitch() const;

  void setPitch(const float t_pitch);
  [[nodiscard]] float getFov() const;
  void setFov(float t_fov);

  [[nodiscard]] float getAspectRatio() const;
  void setAspectRatio(float t_aspectRatio);

  void setAspectRatio(ImageSize t_size);

  [[nodiscard]] float getNearPlane() const;
  void setNearPlane(float t_near);
  [[nodiscard]] float getFarPlane() const;
  void setFarPlane(float t_far);

  [[nodiscard]] glm::mat4 getViewTransform() const override;
  [[nodiscard]] glm::mat4 getProjectionTransform() const;

  void updateUniforms(Shader &t_shader) override;

  // Moves the camera in the given direction by the given amount.
  void move(ECameraDirection t_direction, float t_velocity);
  // Rotates the camera by the given yaw and pitch offsets. If constrainPitch is
  // true, pitch is clamped when nearPlane the up and down directions.
  void rotate(float t_yawOffset, float t_pitchOffset, bool t_constrainPitch = true);
  // Changes the zoom level of the FoV by the given offset. Alternative to
  // setFov().
  void zoom(float t_offset);

private:
  void updateCameraVectors();

  glm::vec3 m_position;
  glm::vec3 m_front{};
  glm::vec3 m_up{};
  glm::vec3 m_right{};
  glm::vec3 m_worldUp;

  // The **clockwise** angle that the camera is facing, measured from the
  // positive X axis.
  float m_yaw;
  float m_pitch;
  float m_fov;

  float m_aspectRatio;
  float m_near;
  float m_far;
};

struct MouseDelta {
  float xoffset;
  float yoffset;
};

// Interface for camera controllers.
class CameraControls {
public:
  virtual ~CameraControls() = default;
  virtual void resizeWindow(int t_width, int t_height) = 0;
  virtual void scroll(Camera &t_camera, double t_xoffset, double t_yoffset,
                      bool t_mouseCaptured) = 0;
  virtual void mouseMove(Camera &t_camera, double t_xpos, double t_ypos,
                         bool t_mouseCaptured) = 0;
  virtual void mouseButton(Camera &t_camera, int t_button, int t_action, int t_mods,
                           bool t_mouseCaptured) = 0;
  virtual void processInput(GLFWwindow *t_window, Camera &t_camera,
                            float t_deltaTime) = 0;

  // Base helpers; call these from subclasses.
  void handleDragStartEnd(int t_button, int t_action);
  MouseDelta calculateMouseDelta(double t_xPos, double t_yPos);

  [[nodiscard]] float getSpeed() const { return speed; }
  static void setSpeed(float t_speed) { t_speed = t_speed; }
  [[nodiscard]] float getSensitivity() const { return sensitivity; }
  static void setSensitivity(float t_sensitivity) { t_sensitivity = t_sensitivity; }

protected:
  static constexpr float defaultSpeed = 5.0f;
  static constexpr float defaultSensitivity = 0.1f;

  // Whether input updates should use the last mouse positions to calculate
  // delta, or to start from the current positions.
  bool initialized = false;
  // Whether a mouse is being click+dragged.
  bool dragging = false;
  float lastMouseX_;
  float lastMouseY_;

  // General setting for camera movement speed.
  float speed = defaultSpeed;
  // General setting for camera sensitivity.
  float sensitivity = defaultSensitivity;
};

// Camera controls that implement a fly mode, similar to DCC tools.
class FlyCameraControls final : public CameraControls {
public:
  ~FlyCameraControls() override;
  void resizeWindow(int t_width, int t_height) override;
  void scroll(Camera &t_camera, double t_xOffset, double t_yOffset,
              bool t_mouseCaptured) override;
  void mouseMove(Camera &t_camera, double t_xPos, double t_yPos,
                 bool t_mouseCaptured) override;
  void mouseButton(Camera &t_camera, int t_button, int t_action, int t_mods,
                   bool t_mouseCaptured) override;
  void processInput(GLFWwindow *t_window, Camera &t_camera,
                    float t_deltaTime) override;
};

// Camera controls that implement an orbit mode at a focal point.
class OrbitCameraControls final : public CameraControls {
public:
  explicit OrbitCameraControls(Camera &t_camera, glm::vec3 t_center = glm::vec3(0.0f));
  ~OrbitCameraControls() override;
  void resizeWindow(int t_width, int t_height) override;
  void scroll(Camera &t_camera, double t_xOffset, double t_yOffset,
              bool t_mouseCaptured) override;
  void mouseMove(Camera &t_camera, double t_xPos, double t_yPos,
                 bool t_mouseCaptured) override;
  void mouseButton(Camera &t_camera, int t_button, int t_action, int t_mods,
                   bool t_mouseCaptured) override;
  void processInput(GLFWwindow *t_window, Camera &t_camera,
                    float t_deltaTime) override;

  [[nodiscard]] glm::vec3 getCenter() const;
  void setCenter(const glm::vec3 t_center);

  // Updates the camera position and orientation to match the current orbit
  // position.
  void updateCamera(Camera &t_camera) const;

private:
  static constexpr float MIN_RADIUS = 0.1f;
  static constexpr float MAX_RADIUS = 100.0f;
  // The center point that we're orbiting around.
  glm::vec3 m_center;
  // The distance from the center.
  float m_radius;
  // The **clockwise** angle in degrees that the camera is rotated about the
  // center.
  float m_azimuth;
  // The angle in degrees that the camera is tilted up or down.
  float m_altitude;
};
