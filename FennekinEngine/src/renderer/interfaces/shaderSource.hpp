#pragma once

class ShaderSource {
public:
  virtual ~ShaderSource() = default;

  explicit ShaderSource(const char* t_value) : value(t_value) {}
  [[nodiscard]] virtual bool isPath() const = 0;
  const char* value;
};
