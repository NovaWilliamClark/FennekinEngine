#pragma once

#pragma fnk_include < lighting.frag>
#pragma fnk_include < pbr.frag>

#ifndef fnk_MAX_DIRECTIONAL_LIGHTS
#define fnk_MAX_DIRECTIONAL_LIGHTS 10
#endif
uniform QrkDirectionalLight fnk_directionalLights[fnk_MAX_DIRECTIONAL_LIGHTS];
uniform int fnk_directionalLightCount;

#ifndef fnk_MAX_POINT_LIGHTS
#define fnk_MAX_POINT_LIGHTS 10
#endif
uniform QrkPointLight fnk_pointLights[fnk_MAX_POINT_LIGHTS];
uniform int fnk_pointLightCount;

#ifndef fnk_MAX_SPOT_LIGHTS
#define fnk_MAX_SPOT_LIGHTS 10
#endif
uniform QrkSpotLight fnk_spotLights[fnk_MAX_SPOT_LIGHTS];
uniform int fnk_spotLightCount;