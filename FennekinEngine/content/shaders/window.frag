#pragma once

// TODO: This file should be .glsl

uniform float fnk_deltaTime;
// TODO: Replace these uniforms with a vec2.
uniform int fnk_windowWidth;
uniform int fnk_windowHeight;

bool fnk_isWindowLeftHalf() { return gl_FragCoord.x < (fnk_windowWidth / 2); }
bool fnk_isWindowRightHalf() { return gl_FragCoord.x >= (fnk_windowWidth / 2); }
bool fnk_isWindowBottomHalf() {
  return gl_FragCoord.y < (fnk_windowHeight / 2);
}
bool fnk_isWindowTopHalf() { return gl_FragCoord.y >= (fnk_windowHeight / 2); }
