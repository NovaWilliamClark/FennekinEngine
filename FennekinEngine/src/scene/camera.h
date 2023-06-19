#pragma once

// clang-format off
// Must precede glfw/glad, to include OpenGL functions.
#include "core/core.h"
// clang-format on

#include <GLFW/glfw3.h>
#include <gl/glew.h>
#include "scene/lighting/light.h"
#include "rendering/interfaces/screen.h"
#include "rendering/resources/shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

enum class CameraDirection {
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

class Camera : public UniformSource, public ViewSource {
 public:
  // Constructs a new Camera. Angular values should be provided in degrees.
  explicit Camera(glm::vec3 position = glm::vec3(0.0f),
                  glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f),
                  float yaw = DEFAULT_YAW, float pitch = DEFAULT_PITCH,
                  float fov = DEFAULT_FOV, float aspectRatio = DEFAULT_ASPECT_RATIO,
                  float near = DEFAULT_NEAR, float far = DEFAULT_FAR);
  ~Camera() override = default;

  void lookAt(glm::vec3 center);

  glm::vec3 getPosition() const { return m_position; }
  void setPosition(glm::vec3 position) { m_position = position; }
  float getYaw() const { return m_yaw; }
  void setYaw(float yaw) {
    m_yaw = yaw;
    updateCameraVectors();
  }
  float getPitch() const { return m_pitch; }
  void setPitch(float pitch) {
    m_pitch = pitch;
    updateCameraVectors();
  }
  float getFov() const { return m_fov; }
  void setFov(float fov) { m_fov = fov; }

  float getAspectRatio() const { return m_aspectRatio; }
  void setAspectRatio(float aspectRatio) { m_aspectRatio = aspectRatio; }
  void setAspectRatio(ImageSize size) {
    m_aspectRatio = size.width / static_cast<float>(size.height);
  }

  float getNearPlane() const { return m_near; }
  void setNearPlane(float near) { m_near = near; }
  float getFarPlane() const { return m_far; }
  void setFarPlane(float far) { m_far = far; }

  glm::mat4 getViewTransform() const override;
  glm::mat4 getProjectionTransform() const;

  void updateUniforms(Shader& shader) override;

  // Moves the camera in the given direction by the given amount.
  void move(CameraDirection direction, float velocity);
  // Rotates the camera by the given yaw and pitch offsets. If constrainPitch is
  // true, pitch is clamped when nearPlane the up and down directions.
  void rotate(float yawOffset, float pitchOffset, bool constrainPitch = true);
  // Changes the zoom level of the FoV by the given offset. Alternative to
  // setFov().
  void zoom(float fovOffset);

 private:
  void updateCameraVectors();

  glm::vec3 m_position;
  glm::vec3 m_front;
  glm::vec3 m_up;
  glm::vec3 m_right;
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
  virtual void resizeWindow(int width, int height) = 0;
  virtual void scroll(Camera& camera, double xoffset, double yoffset,
                      bool mouseCaptured) = 0;
  virtual void mouseMove(Camera& camera, double xpos, double ypos,
                         bool mouseCaptured) = 0;
  virtual void mouseButton(Camera& camera, int button, int action, int mods,
                           bool mouseCaptured) = 0;
  virtual void processInput(GLFWwindow* window, Camera& camera,
                            float deltaTime) = 0;

  // Base helpers; call these from subclasses.
  void handleDragStartEnd(int button, int action);
  MouseDelta calculateMouseDelta(double xpos, double ypos);

  [[nodiscard]] float getSpeed() const { return speed; }
  void setSpeed(float speed) { speed = speed; }
  float getSensitivity() const { return sensitivity; }
  void setSensitivity(float sensitivity) { sensitivity = sensitivity; }

 protected:
  static constexpr float DEFAULT_SPEED = 5.0f;
  static constexpr float DEFAULT_SENSITIVITY = 0.1f;

  // Whether input updates should use the last mouse positions to calculate
  // delta, or to start from the current positions.
  bool initialized = false;
  // Whether a mouse is being click+dragged.
  bool dragging = false;
  float lastMouseX_;
  float lastMouseY_;

  // General setting for camera movement speed.
  float speed = DEFAULT_SPEED;
  // General setting for camera sensitivity.
  float sensitivity = DEFAULT_SENSITIVITY;
};

// Camera controls that implement a fly mode, similar to DCC tools.
class FlyCameraControls : public CameraControls {
 public:
  virtual ~FlyCameraControls() = default;
  void resizeWindow(int width, int height) override;
  void scroll(Camera& camera, double xoffset, double yoffset,
              bool mouseCaptured) override;
  void mouseMove(Camera& camera, double xpos, double ypos,
                 bool mouseCaptured) override;
  void mouseButton(Camera& camera, int button, int action, int mods,
                   bool mouseCaptured) override;
  void processInput(GLFWwindow* window, Camera& camera,
                    float deltaTime) override;
};

// Camera controls that implement an orbit mode at a focal point.
class OrbitCameraControls : public CameraControls {
 public:
  OrbitCameraControls(Camera& camera, glm::vec3 center = glm::vec3(0.0f));
  virtual ~OrbitCameraControls() = default;
  void resizeWindow(int width, int height) override;
  void scroll(Camera& camera, double xoffset, double yoffset,
              bool mouseCaptured) override;
  void mouseMove(Camera& camera, double xpos, double ypos,
                 bool mouseCaptured) override;
  void mouseButton(Camera& camera, int button, int action, int mods,
                   bool mouseCaptured) override;
  void processInput(GLFWwindow* window, Camera& camera,
                    float deltaTime) override;

  glm::vec3 getCenter() const { return m_center; }
  void setCenter(glm::vec3 center) { m_center = center; }

  // Updates the camera position and orientation to match the current orbit
  // position.
  void updateCamera(Camera& camera);

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

