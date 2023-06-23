#include "camera.hpp"

constexpr float POLAR_CAP = 90.0f - 0.1f;

Camera::Camera(glm::vec3 t_position, glm::vec3 t_worldUp, float t_yaw, float t_pitch,
               float t_fov, float t_aspectRatio, float t_near, float t_far)
    : m_position(t_position), m_worldUp(t_worldUp), m_yaw(t_yaw), m_pitch(t_pitch),
      m_fov(t_fov), m_aspectRatio(t_aspectRatio), m_near(t_near), m_far(t_far) {
  updateCameraVectors();
}

Camera::~Camera() = default;

glm::vec3 Camera::getPosition() const { return m_position; }

void Camera::setPosition(const glm::vec3 t_position) { m_position = t_position; }

float Camera::getYaw() const { return m_yaw; }

void Camera::setYaw(const float t_yaw) {
  m_yaw = t_yaw;
  updateCameraVectors();
}

float Camera::getPitch() const { return m_pitch; }

void Camera::setPitch(const float t_pitch) {
  m_pitch = t_pitch;
  updateCameraVectors();
}

float Camera::getFov() const { return m_fov; }

void Camera::setFov(const float t_fov) { m_fov = t_fov; }

float Camera::getAspectRatio() const { return m_aspectRatio; }

void Camera::setAspectRatio(const float t_aspectRatio) { m_aspectRatio = t_aspectRatio; }
void Camera::setAspectRatio(const ImageSize t_size) {
  m_aspectRatio = t_size.width / static_cast<float>(t_size.height);
}

float Camera::getNearPlane() const { return m_near; }

void Camera::setNearPlane(const float t_near) { m_near = t_near; }

float Camera::getFarPlane() const { return m_far; }

void Camera::setFarPlane(const float t_far) { m_far = t_far; }

void Camera::updateCameraVectors() {
  glm::vec3 front;
  front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
  front.y = sin(glm::radians(m_pitch));
  front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

  m_front = glm::normalize(front);
  m_right = glm::normalize(glm::cross(m_front, m_worldUp));
  m_up = glm::normalize(glm::cross(m_right, m_front));
}

void Camera::lookAt(const glm::vec3 t_center) {
  const glm::vec3 dir = glm::normalize(t_center - m_position);
  m_pitch = glm::degrees(asin(dir.y));
  m_yaw = glm::mod<float>(glm::degrees(atan2(dir.x, dir.z)) * -1.0f, 360.0f) +
          90.0f;
  updateCameraVectors();
}

glm::mat4 Camera::getViewTransform() const {
  const glm::vec3 center = m_position + m_front;
  return glm::lookAt(/*eye=*/m_position, center, m_up);
}

glm::mat4 Camera::getProjectionTransform() const {
  return glm::perspective(glm::radians(getFov()), m_aspectRatio, m_near, m_far);
}

void Camera::updateUniforms(Shader &t_shader) {
  t_shader.setMat4("view", getViewTransform());
  t_shader.setMat4("projection", getProjectionTransform());
}

void Camera::move(const ECameraDirection t_direction, const float t_velocity) {
  switch (t_direction) {
  case ECameraDirection::FORWARD:
    m_position += m_front * t_velocity;
    break;
  case ECameraDirection::BACKWARD:
    m_position -= m_front * t_velocity;
    break;
  case ECameraDirection::LEFT:
    m_position -= m_right * t_velocity;
    break;
  case ECameraDirection::RIGHT:
    m_position += m_right * t_velocity;
    break;
  case ECameraDirection::UP:
    m_position += m_up * t_velocity;
    break;
  case ECameraDirection::DOWN:
    m_position -= m_up * t_velocity;
    break;
  }
}

void Camera::rotate(const float t_yawOffset, const float t_pitchOffset, const bool t_constrainPitch) {
  // Constrain yaw to be 0-360 to avoid floating point error.
  m_yaw = glm::mod(m_yaw + t_yawOffset, 360.0f);
  m_pitch += t_pitchOffset;

  if (t_constrainPitch) {
    m_pitch = glm::clamp(m_pitch, -POLAR_CAP, POLAR_CAP);
  }

  updateCameraVectors();
}

void Camera::zoom(const float t_offset) {
  m_fov = glm::clamp(m_fov - t_offset, MIN_FOV, MAX_FOV);
}

void CameraControls::handleDragStartEnd(const int t_button, const int t_action) {
  if (t_button == GLFW_MOUSE_BUTTON_LEFT && t_action == GLFW_PRESS) {
    dragging = true;
    initialized = false;
  } else if (t_button == GLFW_MOUSE_BUTTON_LEFT && t_action == GLFW_RELEASE) {
    dragging = false;
  }
}

MouseDelta CameraControls::calculateMouseDelta(const double t_xPos, const double t_yPos) {
  if (!initialized) {
    lastMouseX_ = t_xPos;
    lastMouseY_ = t_yPos;
    initialized = true;
  }
  const float xOffset = t_xPos - lastMouseX_;
  // Reversed since y-coordinates range from bottom to top.
  const float yOffset = lastMouseY_ - t_yPos;
  lastMouseX_ = t_xPos;
  lastMouseY_ = t_yPos;
  return MouseDelta{xOffset, yOffset};
}

FlyCameraControls::~FlyCameraControls() = default;
void FlyCameraControls::resizeWindow(int t_width, int t_height) {}

void FlyCameraControls::scroll(Camera &t_camera, double t_xOffset, const double t_yOffset,
                               bool t_mouseCaptured) {
  // Always respond to scroll.
  t_camera.zoom(t_yOffset * sensitivity);
}

void FlyCameraControls::mouseMove(Camera &t_camera, const double t_xPos, const double t_yPos, const bool t_mouseCaptured) {
  // Only move when dragging, or when the mouse is captured.
  if (!(dragging || t_mouseCaptured)) {
    return;
  }

  const auto [xOffset, yOffset] = calculateMouseDelta(t_xPos, t_yPos);

  t_camera.rotate(xOffset * sensitivity, yOffset * sensitivity);
}

void FlyCameraControls::mouseButton(Camera &t_camera, const int t_button, const int t_action,
                                    int t_mods, bool t_mouseCaptured) {
  handleDragStartEnd(t_button, t_action);
}

void FlyCameraControls::processInput(GLFWwindow *t_window, Camera &t_camera, const float t_deltaTime) {
  const float velocity = speed * t_deltaTime;
  if (glfwGetKey(t_window, GLFW_KEY_W) == GLFW_PRESS) {
    t_camera.move(ECameraDirection::FORWARD, velocity);
  }
  if (glfwGetKey(t_window, GLFW_KEY_A) == GLFW_PRESS) {
    t_camera.move(ECameraDirection::LEFT, velocity);
  }
  if (glfwGetKey(t_window, GLFW_KEY_S) == GLFW_PRESS) {
    t_camera.move(ECameraDirection::BACKWARD, velocity);
  }
  if (glfwGetKey(t_window, GLFW_KEY_D) == GLFW_PRESS) {
    t_camera.move(ECameraDirection::RIGHT, velocity);
  }
  if (glfwGetKey(t_window, GLFW_KEY_E) == GLFW_PRESS) {
    t_camera.move(ECameraDirection::UP, velocity);
  }
  if (glfwGetKey(t_window, GLFW_KEY_Q) == GLFW_PRESS) {
    t_camera.move(ECameraDirection::DOWN, velocity);
  }
}

OrbitCameraControls::OrbitCameraControls(Camera &t_camera, const glm::vec3 t_center)
    : m_center(t_center) {
    const glm::vec3 dir = t_camera.getPosition() - t_center;
  m_radius = glm::clamp(glm::length(dir), MIN_RADIUS, MAX_RADIUS);
    const glm::vec3 normDir = glm::normalize(dir);
  // TODO: Fix this, atan2 args are backwards.
  m_azimuth = glm::mod<float>(glm::degrees(atan2(normDir.x, normDir.z)) * -1.0f,
                              360.0f) +
              90.0f;
  m_altitude =
      glm::clamp<float>(glm::degrees(asin(normDir.y)), -POLAR_CAP, POLAR_CAP);

  updateCamera(t_camera);
}

OrbitCameraControls::~OrbitCameraControls() = default;

glm::vec3 OrbitCameraControls::getCenter() const { return m_center; }

void OrbitCameraControls::setCenter(const glm::vec3 t_center) { m_center = t_center; }

void OrbitCameraControls::resizeWindow(int t_width, int t_height) {}

void OrbitCameraControls::scroll(Camera &t_camera, double t_xOffset, const double t_yOffset,
                                 bool t_mouseCaptured) {
  m_radius = glm::clamp(m_radius - static_cast<float>(t_yOffset * sensitivity),
                        MIN_RADIUS, MAX_RADIUS);
  updateCamera(t_camera);
}

void OrbitCameraControls::mouseMove(Camera &t_camera, const double t_xPos, const double t_yPos, const bool t_mouseCaptured) {
  // Only move when dragging, or when the mouse is captured.
  if (!(dragging || t_mouseCaptured)) {
    return;
  }

  auto [xOffset, yOffset] = calculateMouseDelta(t_xPos, t_yPos);

  // Constrain azimuth to be 0-360 to avoid floating point error.
  m_azimuth = glm::mod(m_azimuth + xOffset * sensitivity, 360.0f);
  m_altitude = glm::clamp(m_altitude - yOffset * sensitivity, -POLAR_CAP,
                          POLAR_CAP);

  updateCamera(t_camera);
}

void OrbitCameraControls::mouseButton(Camera &t_camera, const int t_button, const int t_action,
                                      int t_mods, bool t_mouseCaptured) {
  handleDragStartEnd(t_button, t_action);
}

void OrbitCameraControls::processInput(GLFWwindow *t_window, Camera &t_camera,
                                       float t_deltaTime) {}

void OrbitCameraControls::updateCamera(Camera &t_camera) const {
  // Compute camera position.
  glm::vec3 cameraPosition;
  cameraPosition.x = m_center.x + m_radius * cos(glm::radians(m_altitude)) *
                                      cos(glm::radians(m_azimuth));
  cameraPosition.y = m_center.y + m_radius * sin(glm::radians(m_altitude));
  cameraPosition.z = m_center.z + m_radius * cos(glm::radians(m_altitude)) *
                                      sin(glm::radians(m_azimuth));

  t_camera.setPosition(cameraPosition);
  t_camera.lookAt(m_center);
}
