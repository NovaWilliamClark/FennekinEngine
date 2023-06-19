#include "camera.h"


constexpr float POLAR_CAP = 90.0f - 0.1f;

Camera::Camera(glm::vec3 position, glm::vec3 worldUp, float yaw, float pitch,
               float fov, float aspectRatio, float near, float far)
    : m_position(position),
      m_worldUp(worldUp),
      m_yaw(yaw),
      m_pitch(pitch),
      m_fov(fov),
      m_aspectRatio(aspectRatio),
      m_near(near),
      m_far(far) {
  updateCameraVectors();
}

void Camera::updateCameraVectors() {
  glm::vec3 front;
  front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
  front.y = sin(glm::radians(m_pitch));
  front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

  m_front = glm::normalize(front);
  m_right = glm::normalize(glm::cross(m_front, m_worldUp));
  m_up = glm::normalize(glm::cross(m_right, m_front));
}

void Camera::lookAt(glm::vec3 center) {
  glm::vec3 dir = glm::normalize(center - m_position);
  m_pitch = glm::degrees(asin(dir.y));
  m_yaw = glm::mod<float>(glm::degrees(atan2(dir.x, dir.z)) * -1.0f, 360.0f) +
         90.0f;
  updateCameraVectors();
}

glm::mat4 Camera::getViewTransform() const {
  glm::vec3 center = m_position + m_front;
  return glm::lookAt(/*eye=*/m_position, center, m_up);
}

glm::mat4 Camera::getProjectionTransform() const {
  return glm::perspective(glm::radians(getFov()), m_aspectRatio, m_near, m_far);
}

void Camera::updateUniforms(Shader& shader) {
  shader.setMat4("view", getViewTransform());
  shader.setMat4("projection", getProjectionTransform());
}

void Camera::move(CameraDirection direction, float velocity) {
  switch (direction) {
    case CameraDirection::FORWARD:
      m_position += m_front * velocity;
      break;
    case CameraDirection::BACKWARD:
      m_position -= m_front * velocity;
      break;
    case CameraDirection::LEFT:
      m_position -= m_right * velocity;
      break;
    case CameraDirection::RIGHT:
      m_position += m_right * velocity;
      break;
    case CameraDirection::UP:
      m_position += m_up * velocity;
      break;
    case CameraDirection::DOWN:
      m_position -= m_up * velocity;
      break;
  }
}

void Camera::rotate(float yawOffset, float pitchOffset, bool constrainPitch) {
  // Constrain yaw to be 0-360 to avoid floating point error.
  m_yaw = glm::mod(m_yaw + yawOffset, 360.0f);
  m_pitch += pitchOffset;

  if (constrainPitch) {
    m_pitch = glm::clamp(m_pitch, -POLAR_CAP, POLAR_CAP);
  }

  updateCameraVectors();
}

void Camera::zoom(float offset) {
  m_fov = glm::clamp(m_fov - offset, MIN_FOV, MAX_FOV);
}

void CameraControls::handleDragStartEnd(int button, int action) {
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    dragging = true;
    initialized = false;
  } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
    dragging = false;
  }
}

MouseDelta CameraControls::calculateMouseDelta(double xpos, double ypos) {
  if (!initialized) {
    lastMouseX_ = xpos;
    lastMouseY_ = ypos;
    initialized = true;
  }
  float xoffset = xpos - lastMouseX_;
  // Reversed since y-coordinates range from bottom to top.
  float yoffset = lastMouseY_ - ypos;
  lastMouseX_ = xpos;
  lastMouseY_ = ypos;
  return MouseDelta{xoffset, yoffset};
}

void FlyCameraControls::resizeWindow(int width, int height) {}

void FlyCameraControls::scroll(Camera& camera, double xoffset, double yoffset,
                               bool mouseCaptured) {
  // Always respond to scroll.
  camera.zoom(yoffset * sensitivity);
}

void FlyCameraControls::mouseMove(Camera& camera, double xpos, double ypos,
                                  bool mouseCaptured) {
  // Only move when dragging, or when the mouse is captured.
  if (!(dragging || mouseCaptured)) {
    return;
  }

  MouseDelta delta = calculateMouseDelta(xpos, ypos);

  camera.rotate(delta.xoffset * sensitivity, delta.yoffset * sensitivity);
}

void FlyCameraControls::mouseButton(Camera& camera, int button, int action,
                                    int mods, bool mouseCaptured) {
  handleDragStartEnd(button, action);
}

void FlyCameraControls::processInput(GLFWwindow* window, Camera& camera,
                                     float deltaTime) {
  float velocity = speed * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    camera.move(CameraDirection::FORWARD, velocity);
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    camera.move(CameraDirection::LEFT, velocity);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    camera.move(CameraDirection::BACKWARD, velocity);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    camera.move(CameraDirection::RIGHT, velocity);
  }
  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
    camera.move(CameraDirection::UP, velocity);
  }
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
    camera.move(CameraDirection::DOWN, velocity);
  }
}

OrbitCameraControls::OrbitCameraControls(Camera& camera, glm::vec3 center)
    : m_center(center) {
  glm::vec3 dir = camera.getPosition() - center;
  m_radius = glm::clamp(glm::length(dir), MIN_RADIUS, MAX_RADIUS);
  glm::vec3 normdir = glm::normalize(dir);
  // TODO: Fix this, atan2 args are backwards.
  m_azimuth = glm::mod<float>(glm::degrees(atan2(normdir.x, normdir.z)) * -1.0f,
                             360.0f) +
             90.0f;
  m_altitude =
      glm::clamp<float>(glm::degrees(asin(normdir.y)), -POLAR_CAP, POLAR_CAP);

  updateCamera(camera);
}

void OrbitCameraControls::resizeWindow(int width, int height) {}

void OrbitCameraControls::scroll(Camera& camera, double xoffset, double yoffset,
                                 bool mouseCaptured) {
  m_radius = glm::clamp(m_radius - static_cast<float>(yoffset * sensitivity),
                       MIN_RADIUS, MAX_RADIUS);
  updateCamera(camera);
}

void OrbitCameraControls::mouseMove(Camera& camera, double xpos, double ypos,
                                    bool mouseCaptured) {
  // Only move when dragging, or when the mouse is captured.
  if (!(dragging || mouseCaptured)) {
    return;
  }

  MouseDelta delta = calculateMouseDelta(xpos, ypos);

  // Constrain azimuth to be 0-360 to avoid floating point error.
  m_azimuth = glm::mod(m_azimuth + delta.xoffset * sensitivity, 360.0f);
  m_altitude = glm::clamp(m_altitude - delta.yoffset * sensitivity, -POLAR_CAP,
                         POLAR_CAP);

  updateCamera(camera);
}

void OrbitCameraControls::mouseButton(Camera& camera, int button, int action,
                                      int mods, bool mouseCaptured) {
  handleDragStartEnd(button, action);
}

void OrbitCameraControls::processInput(GLFWwindow* window, Camera& camera,
                                       float deltaTime) {}

void OrbitCameraControls::updateCamera(Camera& camera) {
  // Compute camera position.
  glm::vec3 cameraPosition;
  cameraPosition.x = m_center.x + m_radius * cos(glm::radians(m_altitude)) *
                                     cos(glm::radians(m_azimuth));
  cameraPosition.y = m_center.y + m_radius * sin(glm::radians(m_altitude));
  cameraPosition.z = m_center.z + m_radius * cos(glm::radians(m_altitude)) *
                                     sin(glm::radians(m_azimuth));

  camera.setPosition(cameraPosition);
  camera.lookAt(m_center);
}
