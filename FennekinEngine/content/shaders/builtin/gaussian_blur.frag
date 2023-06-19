#version 460 core
#pragma fnk_include < post_processing.frag>
in vec2 texCoords;

out vec4 fragColor;

uniform sampler2D screenTexture;
uniform bool horizontal;

void main() {
  fragColor = fnk_gaussianBlurOnePass(screenTexture, texCoords, horizontal);
}
