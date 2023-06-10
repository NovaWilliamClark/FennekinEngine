#pragma once
#include <glm/glm.hpp>

/**
 * @class Camera
 * @brief Represents a camera in a 3D space.
 */
class Camera
{
public:
	/**
	 * @brief Default constructor for the Camera class.
	 */
	Camera();

	/**
	 * @brief Updates the camera state.
	 *
	 * @param t_deltaTime The time elapsed since the last update.
	 */
	void update(float t_deltaTime);

	/**
	 * @brief Updates the camera orientation.
	 */
	void updateOrientation();

	/**
	 * @brief Updates the camera translation.
	 */
	void updateTranslation();

	/**
	 * @brief Gets the projection view matrix.
	 *
	 * @return The projection view matrix.
	 */
	[[nodiscard]] glm::mat4 getProjectionViewMatrix() const;

	/**
	 * @brief Gets the view matrix.
	 *
	 * @return The view matrix.
	 */
	[[nodiscard]] glm::mat4 getViewMatrix() const;

	/**
	 * @brief Gets the projection matrix.
	 *
	 * @return The projection matrix.
	 */
	[[nodiscard]] glm::mat4 getProjectionMatrix() const;

	/**
	 * @brief Sets the near plane distance.
	 *
	 * @param t_nearPlane The near plane distance.
	 */
	void setNearPlane(float_t t_nearPlane);

	/**
	 * @brief Sets the far plane distance.
	 *
	 * @param t_farPlane The far plane distance.
	 */
	void setFarPlane(float_t t_farPlane);

	/**
	 * @brief Sets the field of view.
	 *
	 * @param t_degrees The field of view in degrees.
	 */
	void setFieldOfView(float_t t_degrees);

	/**
	 * @brief Sets the viewport aspect ratio.
	 *
	 * @param t_aspectRatio The viewport aspect ratio.
	 */
	void setViewportAspectRatio(float_t t_aspectRatio);

private:
	glm::vec3 m_front;    // The front direction of the camera.
	glm::vec3 m_right;    // The right direction of the camera.
	glm::vec3 m_up;       // The up direction of the camera.
	glm::vec3 m_worldUp;  // The world up direction.
	glm::vec3 m_position; // The position of the camera.

	float_t m_maximumPitch;        // The maximum pitch angle.
	float_t m_pitch;               // The pitch angle.
	float_t m_yaw;                 // The yaw angle.
	float_t m_fieldOfView;         // The field of view.
	float_t m_nearPlane;           // The near plane distance.
	float_t m_farPlane;            // The far plane distance.
	float_t m_viewportAspectRatio; // The viewport aspect ratio.

	float_t m_cameraSpeed{}; // The camera speed.
};