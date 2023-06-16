#pragma once
#include <glm/glm.hpp>
#include <glfw/glfw3.h>
#include <gl/glew.h>

#include "core/Window.hpp"

#include "renderer/interfaces/UniformSource.hpp"
#include "renderer/interfaces/ViewSource.hpp"

#include "utilities/constants.hpp"


enum class ECameraDirection {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN,
};


/**
 * @class Camera
 * @brief Represents a camera in a 3D space.
 */
class Camera final : public UniformSource, public ViewSource
{
public:
    Camera();
    Camera(glm::vec3 t_position, glm::vec3 t_worldUp, float_t t_yaw, float_t t_pitch, float_t t_fov, float_t t_aspectRatio, float_t t_near, float_t t_far);
    ~Camera() override;


    [[nodiscard]] glm::vec3 getPosition() const;
    void setPosition(glm::vec3 t_position);

    [[nodiscard]] float_t getYaw() const;
    void setYaw(float_t t_yaw);

    [[nodiscard]] float_t getPitch() const;
    void setPitch(float_t t_pitch);

    [[nodiscard]] float_t getFov() const;
    void setFov(float_t t_fov);

    [[nodiscard]] float_t getAspectRatio() const;
    void setAspectRatio(const float_t t_aspectRatio);

    void setAspectRatio(const ImageSize t_size);

    [[nodiscard]] float_t getNearPlane() const;
    void setNearPlane(const float_t t_near);
    [[nodiscard]] float_t getFarPlane() const;
    void setFarPlane(const float_t t_far);

    [[nodiscard]] glm::mat4 getViewTransform() const override;
    [[nodiscard]] glm::mat4 getProjectionTransform() const;

    void updateUniforms(Shader& t_shader, float_t t_deltaTime) override;

    // Moves the camera in the given direction by the given amount.
    void move(ECameraDirection t_direction, float_t t_velocity);
    // Rotates the camera by the given yaw and pitch offsets. If constrainPitch is
    // true, pitch is clamped when near the up and down directions.
    void rotate(float_t t_yawOffset, float_t t_pitchOffset, bool t_constrainPitch = true);
    // Changes the zoom level of the FoV by the given offset. Alternative to
    // setFov().
    void zoom(float_t t_fovOffset);

private:
    void updateCameraVectors();

    glm::vec3 m_position{};
    glm::vec3 m_front{};
    glm::vec3 m_up{};
    glm::vec3 m_right{};
    glm::vec3 m_worldUp{};

    float_t m_yaw{};
    float_t m_pitch{};
    float_t m_fov{};

    float_t m_aspectRatio{};
    float_t m_near{};
    float_t m_far{};
};