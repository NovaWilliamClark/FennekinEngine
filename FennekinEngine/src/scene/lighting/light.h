#pragma once

#include "core/debug/exceptions.h"
#include "rendering/resources/shader.h"

#include <glm/glm.hpp>
#include <string>
#include <vector>

class LightException final : public QuarkException {
    using QuarkException::QuarkException;
};

struct Attenuation {
    float constant;
    float linear;
    float quadratic;
};

// TODO: Change this to [0, 0, 1].
constexpr Attenuation DEFAULT_ATTENUATION = {1.0f, 0.09f, 0.032f};
constexpr glm::vec3 DEFAULT_DIFFUSE = glm::vec3(0.5f, 0.5f, 0.5f);
constexpr glm::vec3 DEFAULT_SPECULAR = glm::vec3(1.0f, 1.0f, 1.0f);

constexpr float DEFAULT_INNER_ANGLE = glm::radians(10.5f);
constexpr float DEFAULT_OUTER_ANGLE = glm::radians(19.5f);

enum class ELightType {
    DIRECTIONAL_LIGHT,
    POINT_LIGHT,
    SPOT_LIGHT,
};

class LightRegistry;

class Light {
public:
    virtual ~Light() = default;
    [[nodiscard]] virtual ELightType getLightType() const = 0;

    void setUseViewTransform(bool useViewTransform) {
        useViewTransform = useViewTransform;
    }

    friend LightRegistry;

protected:
    void setLightIdx(unsigned int lightIdx) {
        lightIdx = lightIdx;
        uniformName = getUniformName(lightIdx);
    }

    void checkState() {
        if (hasViewDependentChanged && !hasViewBeenApplied) {
            throw LightException("ERROR::LIGHT::VIEW_CHANGED\n"
                                 "Light state changed without re-applying view transform.");
        }
    }

    void resetChangeDetection() {
        hasViewDependentChanged = false;
        hasLightChanged = false;
        hasViewBeenApplied = false;
    }

    [[nodiscard]] virtual std::string getUniformName(unsigned int lightIdx) const = 0;
    virtual void updateUniforms(Shader& shader) = 0;
    virtual void applyViewTransform(const glm::mat4& view) = 0;

    unsigned int lightIdx{};
    std::string uniformName;

    // Whether the light's position uniforms should be in view space. If false,
    // the positions are instead in world space.
    bool useViewTransform = true;
    // Start as `true` so that initial uniform values get set.
    bool hasViewDependentChanged = true;
    bool hasLightChanged = true;

    bool hasViewBeenApplied = false;
};

// A source for the camera view transform.
class ViewSource {
public:
    virtual ~ViewSource() = default;
    [[nodiscard]] virtual glm::mat4 getViewTransform() const = 0;
};

class LightRegistry : public UniformSource {
public:
    virtual ~LightRegistry() = default;
    void addLight(Light* light);
    // Sets the view source used to update the light uniforms. The source is
    // called when updating uniforms.
    void setViewSource(ViewSource* viewSource) {
        viewSource_ = viewSource;
    }
    void updateUniforms(Shader& shader);

    // Applies the view transform to the registered lights. This is automatically
    // called if a view source has been set.
    void applyViewTransform(const glm::mat4& view);

    // Sets whether the registered lights should transform their positions to view
    // space. If false, positions remain in world space when passed to the shader.
    void setUseViewTransform(bool useViewTransform);

private:
    unsigned int m_directionalCount = 0;
    unsigned int m_pointCount = 0;
    unsigned int m_spotCount = 0;

    ViewSource* viewSource_;
    std::vector<Light*> m_lights;
};

class DirectionalLight : public Light {
public:
    explicit DirectionalLight(glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3 diffuse = DEFAULT_DIFFUSE,
                              // TODO: Change this to default to whatever the diffuse was.
                              glm::vec3 specular = DEFAULT_SPECULAR);

    ELightType getLightType() const override {
        return ELightType::DIRECTIONAL_LIGHT;
    }

    glm::vec3 getDirection() const {
        return m_direction;
    }
    void setDirection(glm::vec3 direction) {
        m_direction = direction;
        hasViewDependentChanged = true;
    }
    glm::vec3 getDiffuse() const {
        return m_diffuse;
    }
    void setDiffuse(glm::vec3 diffuse) {
        m_diffuse = diffuse;
        hasLightChanged = true;
    }
    glm::vec3 getSpecular() const {
        return m_specular;
    }
    void setSpecular(glm::vec3 specular) {
        m_specular = specular;
        hasLightChanged = true;
    }

protected:
    std::string getUniformName(unsigned int lightIdx) const override {
        return "fnk_directionalLights[" + std::to_string(lightIdx) + "]";
    }
    void updateUniforms(Shader& shader) override;
    void applyViewTransform(const glm::mat4& view) override;

private:
    glm::vec3 m_direction;
    glm::vec3 m_viewDirection;

    glm::vec3 m_diffuse;
    glm::vec3 m_specular;
};

class PointLight final : public Light {
public:
    explicit PointLight(glm::vec3 position = glm::vec3(0.0f), glm::vec3 diffuse = DEFAULT_DIFFUSE,
                        glm::vec3 specular = DEFAULT_SPECULAR, Attenuation attenuation = DEFAULT_ATTENUATION);

    [[nodiscard]] ELightType getLightType() const override {
        return ELightType::POINT_LIGHT;
    }

    glm::vec3 getPosition() const {
        return m_position;
    }
    void setPosition(glm::vec3 position) {
        m_position = position;
        hasViewDependentChanged = true;
    }
    glm::vec3 getDiffuse() const {
        return m_diffuse;
    }
    void setDiffuse(glm::vec3 diffuse) {
        m_diffuse = diffuse;
        hasLightChanged = true;
    }
    glm::vec3 getSpecular() const {
        return m_specular;
    }
    void setSpecular(glm::vec3 specular) {
        m_specular = specular;
        hasLightChanged = true;
    }
    Attenuation getAttenuation() const {
        return m_attenuation;
    }
    void setAttenuation(Attenuation attenuation) {
        m_attenuation = attenuation;
        hasLightChanged = true;
    }

protected:
    [[nodiscard]] std::string getUniformName(unsigned int lightIdx) const override {
        return "fnk_pointLights[" + std::to_string(lightIdx) + "]";
    }
    void updateUniforms(Shader& shader) override;
    void applyViewTransform(const glm::mat4& view) override;

private:
    glm::vec3 m_position;
    glm::vec3 m_viewPosition;

    glm::vec3 m_diffuse;
    glm::vec3 m_specular;

    Attenuation m_attenuation;
};

class SpotLight : public Light {
public:
    SpotLight(glm::vec3 position = glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f),
              float innerAngle = DEFAULT_INNER_ANGLE, float outerAngle = DEFAULT_OUTER_ANGLE,
              glm::vec3 diffuse = DEFAULT_DIFFUSE, glm::vec3 specular = DEFAULT_SPECULAR,
              Attenuation attenuation = DEFAULT_ATTENUATION);

    ELightType getLightType() const {
        return ELightType::SPOT_LIGHT;
    }

    glm::vec3 getPosition() const {
        return m_position;
    }
    void setPosition(glm::vec3 position) {
        m_position = position;
        hasViewDependentChanged = true;
    }
    glm::vec3 getDirection() const {
        return m_direction;
    }
    void setDirection(glm::vec3 direction) {
        m_direction = direction;
        hasViewDependentChanged = true;
    }
    glm::vec3 getDiffuse() const {
        return m_diffuse;
    }
    void setDiffuse(glm::vec3 diffuse) {
        m_diffuse = diffuse;
        hasLightChanged = true;
    }
    glm::vec3 getSpecular() const {
        return m_specular;
    }
    void setSpecular(glm::vec3 specular) {
        m_specular = specular;
        hasLightChanged = true;
    }
    Attenuation getAttenuation() const {
        return m_attenuation;
    }
    void setAttenuation(Attenuation attenuation) {
        m_attenuation = attenuation;
        hasLightChanged = true;
    }

protected:
    std::string getUniformName(unsigned int lightIdx) const override {
        return "fnk_spotLights[" + std::to_string(lightIdx) + "]";
    }
    void updateUniforms(Shader& shader) override;
    void applyViewTransform(const glm::mat4& view) override;

private:
    glm::vec3 m_position;
    glm::vec3 m_viewPosition;
    glm::vec3 m_direction;
    glm::vec3 m_viewDirection;

    float m_innerAngle;
    float m_outerAngle;

    glm::vec3 m_diffuse;
    glm::vec3 m_specular;

    Attenuation m_attenuation;
};