#pragma once

enum class EShaderType {
  VERTEX,
  FRAGMENT,
  GEOMETRY,
  COMPUTE,
};

struct ShaderSource {
    virtual ~ShaderSource() = default;
    inline explicit ShaderSource(const char* value) : value(value) {}
  const char* value;
    [[nodiscard]] virtual bool isPath() const = 0;
};

struct ShaderInline final : ShaderSource {
  using ShaderSource::ShaderSource;
    [[nodiscard]] bool isPath() const override {
        return false;
    }
};

struct ShaderPath final : ShaderSource {
  using ShaderSource::ShaderSource;
  [[nodiscard]] bool isPath() const override {
      return true;
  }
};
