#pragma once
#include <glm/glm.hpp>

class ViewSource {
public:
    [[nodiscard]] virtual glm::mat4 getViewTransform() const = 0;
    virtual ~ViewSource() = default;
};
