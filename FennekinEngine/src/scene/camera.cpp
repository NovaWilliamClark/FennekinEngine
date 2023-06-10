#include "scene/camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera()
    : m_front(glm::vec3(0.0f, 0.0f, -1.0f)),
    m_right(glm::vec3(1.0f, 0.0f, 0.0f)),
    m_up(glm::vec3(0.0f, 1.0f, 0.0f)),
    m_worldUp(m_up),
    m_position(glm::vec3(0.0f, 0.0f, 0.0f)),
    m_maximumPitch(89.0f),
    m_pitch(0.0f),
    m_yaw(-90.0f),
    m_fieldOfView(45.0f),
    m_nearPlane(0.1f),
    m_farPlane(100.0f),
    m_viewportAspectRatio(16.0f / 9.0f)
{}

void Camera::update(const float t_deltaTime)
{
	glm::vec3 front;
	front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	front.y = sin(glm::radians(m_pitch));
	front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

	m_front = glm::normalize(front);
	m_right = glm::normalize(glm::cross(m_front, m_worldUp));
	m_up = glm::normalize(glm::cross(m_right, m_front));
    // Update camera vectors based on pitch and yaw here.
    // This is a simplified version and may not work correctly in a real application.
}

void Camera::updateOrientation() {
   m_front.x = cos(glm::radians(m_pitch)) * cos(glm::radians(m_yaw));
   m_front.y = sin(glm::radians(m_pitch));
   m_front.z = cos(glm::radians(m_pitch)) * sin(glm::radians(m_yaw));
   m_front = glm::normalize(m_front);
   m_right  = glm::normalize(glm::cross(m_front, m_up));
}
void Camera::updateTranslation() {
    
}

glm::mat4 Camera::getProjectionViewMatrix() const
{
    return getProjectionMatrix() * getViewMatrix();
}

glm::mat4 Camera::getViewMatrix() const
{
    return glm::lookAt(m_position, m_position + m_front, m_up);
}

glm::mat4 Camera::getProjectionMatrix() const
{
    return glm::perspective(glm::radians(m_fieldOfView), m_viewportAspectRatio, m_nearPlane, m_farPlane);
}

void Camera::setNearPlane(const float_t t_nearPlane)
{
    m_nearPlane = t_nearPlane;
}

void Camera::setFarPlane(const float_t t_farPlane)
{
    m_farPlane = t_farPlane;
}

void Camera::setFieldOfView(const float_t t_degrees)
{
    m_fieldOfView = t_degrees;
}

void Camera::setViewportAspectRatio(const float_t t_aspectRatio)
{
    m_viewportAspectRatio = t_aspectRatio;
}

void Camera::setCameraSpeed(const float t_speed) {
    m_cameraSpeed = t_speed;
}

void Camera::setMouseSensitivity(const float t_sensitivity) {
    m_mouseSensitivity = t_sensitivity;
}
