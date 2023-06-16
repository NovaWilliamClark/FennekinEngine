#include "camera.hpp"
#include <glm/ext.hpp>

Camera::Camera() {
    m_position = { 0, 0, 0 };
    m_worldUp = { 0, 1, 0 };
    m_yaw = DEFAULT_YAW;
    m_pitch = DEFAULT_PITCH;
    m_fov = DEFAULT_FOV;
    m_aspectRatio = DEFAULT_ASPECT_RATIO;
    m_near = DEFAULT_NEAR;
    m_far = DEFAULT_FAR;
}

Camera::Camera(glm::vec3 t_position, glm::vec3 t_worldUp, float_t t_yaw, float_t t_pitch,
                float_t t_fov, float_t t_aspectRatio, float_t t_near, float_t t_far) {
    m_position = t_position;
    m_worldUp = t_worldUp;
    m_yaw = t_yaw;
    m_pitch = t_pitch;
    m_fov = t_fov;
    m_aspectRatio = t_aspectRatio;
    m_near = t_near;
    m_far = t_far;
}

Camera::~Camera() = default;

float_t Camera::getAspectRatio() const { return m_aspectRatio; }

void Camera::setAspectRatio(const float_t t_aspectRatio) { m_aspectRatio = t_aspectRatio; }
void Camera::setAspectRatio(const ImageSize t_size) {
    m_aspectRatio = t_size.width / static_cast<float_t>(t_size.height);
}

float_t Camera::getNearPlane() const { return m_near; }

void Camera::setNearPlane(const float_t t_near) { m_near = t_near; }

float_t Camera::getFarPlane() const { return m_far; }

void Camera::setFarPlane(const float_t t_far) { m_far = t_far; }

glm::vec3 Camera::getPosition() const {
    return m_position;
}

void Camera::setPosition(const glm::vec3 t_position) {
    m_position = t_position;
}

float_t Camera::getYaw() const {
    return m_yaw;
}

void Camera::setYaw(const float_t t_yaw) {
    m_yaw = t_yaw;
}

float_t Camera::getPitch() const {
    return m_pitch;
}

void Camera::setPitch(const float_t t_pitch) {
    m_pitch = t_pitch;
}

float_t Camera::getFov() const {
    return m_fov;
}

void Camera::setFov(const float_t t_fov) {
    m_fov = t_fov;
}

glm::mat4 Camera::getViewTransform() const {
    const glm::vec3 center = m_position + m_front;
    return glm::lookAt(m_position, center, m_up);
}

glm::mat4 Camera::getProjectionTransform() const {
    return glm::perspective(glm::radians(getFov()), m_aspectRatio, m_near, m_far);
}

void Camera::updateUniforms(Shader& t_shader, float_t t_deltaTime) {
    // TODO:
    //t_shader.setUniform("view", getViewTransform());
    //t_shader.setUniform("projection", getProjectionTransform());
}

void Camera::move(const ECameraDirection t_direction, const float_t t_velocity) {
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

void Camera::rotate(const float_t t_yawOffset, const float_t t_pitchOffset, const bool t_constrainPitch) {
    // Constrain yaw to be 0-360 to avoid floating point error.
    m_yaw = glm::mod(m_yaw + t_yawOffset, 360.0f);
    m_pitch += t_pitchOffset;

    if (t_constrainPitch) {
        m_pitch = glm::clamp(m_pitch, -POLAR_CAP, POLAR_CAP);
    }

    updateCameraVectors();
}

void Camera::zoom(const float_t t_fovOffset) {
    m_fov = glm::clamp(m_fov - t_fovOffset, MIN_FOV, MAX_FOV);
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
