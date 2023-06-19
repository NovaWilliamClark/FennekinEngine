#version 460 core
#pragma fnk_include < transforms.glsl>
#pragma fnk_include < window.frag>

in vec2 texCoords;

out float fragColor;

uniform sampler2D fnk_ssao;
uniform int fnk_ssaoNoiseTextureSideLength;

void main() {
  vec2 texelSize = 1.0 / vec2(textureSize(fnk_ssao, 0));

  float result = 0.0;
  // We compute a blur based on the original noise texture size used to generate
  // the SSAO.
  int sideOffset = fnk_ssaoNoiseTextureSideLength / 2;
  for (int x = -sideOffset; x < sideOffset; ++x) {
    for (int y = -sideOffset; y < sideOffset; ++y) {
      // Calculate the UV offset based on texel size.
      vec2 texOffset = vec2(float(x), float(y)) * texelSize;
      result += texture(fnk_ssao, texCoords + texOffset).r;
    }
  }
  // Normalize.
  result /= fnk_ssaoNoiseTextureSideLength * fnk_ssaoNoiseTextureSideLength;
  fragColor = result;
}
