#pragma once
#include "renderer/resources/shader.hpp"


class UniformSource {
public:
  virtual ~UniformSource() = default;

  virtual void updateUniforms(Shader& t_shader, float_t t_deltaTime) = 0;
};