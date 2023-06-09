#version 460 core

// Performs upsampling on a texture. This approach is taken from the technique
// used in Call of Duty presented at Siggraph 2014.

in vec2 texCoords;

out vec4 fragColor;

uniform sampler2D fnk_bloomMipChain;
uniform float fnk_filterRadius;

void main() {
  // Unlike when downsampling, we use a fixed radius in UV space rather than
  // basing it off the texel size of the target mip.
  float offsetX = fnk_filterRadius;
  float offsetY = fnk_filterRadius;

  // Take 9 samples around the current texel (cc):
  //
  // tl tt tr
  // ll cc rr
  // bl bb br
  //
  // Convention:
  // - cc: current
  // - tl: top left (etc)
  // - br: bottom right (etc)

  // clang-format off
  vec4 tl = texture(fnk_bloomMipChain, vec2(texCoords.x - offsetX, texCoords.y + offsetY));
  vec4 tt = texture(fnk_bloomMipChain, vec2(texCoords.x,           texCoords.y + offsetY));
  vec4 tr = texture(fnk_bloomMipChain, vec2(texCoords.x + offsetX, texCoords.y + offsetY));

  vec4 ll = texture(fnk_bloomMipChain, vec2(texCoords.x - offsetX, texCoords.y          ));
  vec4 cc = texture(fnk_bloomMipChain, vec2(texCoords.x,           texCoords.y          ));
  vec4 rr = texture(fnk_bloomMipChain, vec2(texCoords.x + offsetX, texCoords.y          ));

  vec4 bl = texture(fnk_bloomMipChain, vec2(texCoords.x - offsetX, texCoords.y - offsetY));
  vec4 bb = texture(fnk_bloomMipChain, vec2(texCoords.x,           texCoords.y - offsetY));
  vec4 br = texture(fnk_bloomMipChain, vec2(texCoords.x + offsetX, texCoords.y - offsetY));
  // clang-format on

  // Apply weighted distribution by using a 3x3 tent filter:
  // | 1 2 1 |
  // | 2 4 2 | * 1/16
  // | 1 2 1 |
  fragColor = cc * 4.0;
  fragColor += (tt + ll + rr + bb) * 2.0;
  fragColor += (tl + tr + bl + br) * 1.0;
  fragColor *= 1.0 / 16.0;
}
