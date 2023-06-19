#version 460 core
#pragma fnk_include < gamma.frag>
#pragma fnk_include < tone_mapping.frag>

in vec2 texCoords;

out vec4 fragColor;

uniform sampler2D fnk_screenTexture;
uniform sampler2D fnk_bloom;
uniform bool bloom;
uniform float bloomMix;

uniform int toneMapping;
uniform bool gammaCorrect;
uniform float gamma;

void main() {
  vec3 color = texture(fnk_screenTexture, texCoords).rgb;
  if (bloom) {
    vec3 bloomColor = texture(fnk_bloom, texCoords).rgb;
    color = mix(color, bloomColor, bloomMix);
  }

  // Perform tone mapping.
  if (toneMapping == 1) {
    color = fnk_toneMapReinhard(color);
  } else if (toneMapping == 2) {
    color = fnk_toneMapReinhardLuminance(color);
  } else if (toneMapping == 3) {
    color = fnk_toneMapAcesApprox(color);
  } else if (toneMapping == 4) {
    color = fnk_toneMapAMD(color);
  } else {
    // No tone mapping.
  }

  // Perform gamma correction.
  if (gammaCorrect) {
    color = fnk_gammaCorrect(color, gamma);
  }

  fragColor = vec4(color, 1.0);
}
