#pragma once

#pragma fnk_include < lighting.frag>
#pragma fnk_include < pbr.frag>

#ifndef FNK_MAX_DIRECTIONAL_LIGHTS
#define FNK_MAX_DIRECTIONAL_LIGHTS 10
#endif
uniform FnkDirectionalLight fnk_directionalLights[FNK_MAX_DIRECTIONAL_LIGHTS];
uniform int fnk_directionalLightCount;

#ifndef FNK_MAX_POINT_LIGHTS
#define FNK_MAX_POINT_LIGHTS 10
#endif
uniform FnkPointLight fnk_pointLights[FNK_MAX_POINT_LIGHTS];
uniform int fnk_pointLightCount;

#ifndef FNK_MAX_SPOT_LIGHTS
#define FNK_MAX_SPOT_LIGHTS 10
#endif
uniform FnkSpotLight fnk_spotLights[FNK_MAX_SPOT_LIGHTS];
uniform int fnk_spotLightCount;