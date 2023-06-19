#pragma once

#pragma fnk_include < lighting.frag>
#pragma fnk_include < pbr.frag>
#pragma fnk_include < standard_lights.frag>

/** ============================ PBR ============================ **/

/**
 * Calculate shading from all active directional lights.
 * TODO: Accepting shadow here is incorrect, as each light should have its own
 * shadow (or lack thereof, if shadow maps aren't enabled for it). This only
 * works when there's 1 light.
 */
vec3 fnk_shadeAllDirectionalLightsCookTorranceGGX(QrkMaterial material,
                                                  vec3 fragPos, vec3 normal,
                                                  vec2 texCoords,
                                                  float shadow) {
  vec3 result = vec3(0.0);
  for (int i = 0; i < fnk_directionalLightCount; i++) {
    result += fnk_shadeDirectionalLightCookTorranceGGX(
        material, fnk_directionalLights[i], fragPos, normal, texCoords, shadow);
  }
  return result;
}

/** Calculate shading from all active directional lights using deferred data. */
vec3 fnk_shadeAllDirectionalLightsCookTorranceGGXDeferred(
    vec3 albedo, float roughness, float metallic, vec3 fragPos, vec3 normal,
    float shadow) {
  vec3 result = vec3(0.0);
  for (int i = 0; i < fnk_directionalLightCount; i++) {
    result += fnk_shadeDirectionalLightCookTorranceGGXDeferred(
        albedo, roughness, metallic, fnk_directionalLights[i], fragPos, normal,
        shadow);
  }
  return result;
}

/** Calculate shading from all active point lights. */
vec3 fnk_shadeAllPointLightsCookTorranceGGX(QrkMaterial material, vec3 fragPos,
                                            vec3 normal, vec2 texCoords) {
  vec3 result = vec3(0.0);
  for (int i = 0; i < fnk_pointLightCount; i++) {
    result += fnk_shadePointLightCookTorranceGGX(material, fnk_pointLights[i],
                                                 fragPos, normal, texCoords);
  }
  return result;
}

/** Calculate shading from all active point lights using deferred data. */
vec3 fnk_shadeAllPointLightsCookTorranceGGXDeferred(vec3 albedo,
                                                    float roughness,
                                                    float metallic,
                                                    vec3 fragPos, vec3 normal) {
  vec3 result = vec3(0.0);
  for (int i = 0; i < fnk_pointLightCount; i++) {
    result += fnk_shadePointLightCookTorranceGGXDeferred(
        albedo, roughness, metallic, fnk_pointLights[i], fragPos, normal);
  }
  return result;
}

/** Calculate shading from all active spot lights. */
vec3 fnk_shadeAllSpotLightsCookTorranceGGX(QrkMaterial material, vec3 fragPos,
                                           vec3 normal, vec2 texCoords) {
  vec3 result = vec3(0.0);
  for (int i = 0; i < fnk_spotLightCount; i++) {
    result += fnk_shadeSpotLightCookTorranceGGX(material, fnk_spotLights[i],
                                                fragPos, normal, texCoords);
  }
  return result;
}

/** Calculate shading from all active spot lights using deferred data. */
vec3 fnk_shadeAllSpotLightsCookTorranceGGXDeferred(vec3 albedo, float roughness,
                                                   float metallic, vec3 fragPos,
                                                   vec3 normal) {
  vec3 result = vec3(0.0);
  for (int i = 0; i < fnk_spotLightCount; i++) {
    result += fnk_shadeSpotLightCookTorranceGGXDeferred(
        albedo, roughness, metallic, fnk_spotLights[i], fragPos, normal);
  }
  return result;
}

/**
 * Calculate shading from all light sources, except ambient and emission
 * textures.
 */
vec3 fnk_shadeAllLightsCookTorranceGGX(QrkMaterial material, vec3 fragPos,
                                       vec3 normal, vec2 texCoords,
                                       float shadow) {
  vec3 albedo = fnk_extractAlbedo(material, texCoords);

  vec3 directional = fnk_shadeAllDirectionalLightsCookTorranceGGX(
      material, fragPos, normal, texCoords, shadow);
  vec3 point = fnk_shadeAllPointLightsCookTorranceGGX(material, fragPos, normal,
                                                      texCoords);
  vec3 spot = fnk_shadeAllSpotLightsCookTorranceGGX(material, fragPos, normal,
                                                    texCoords);
  return directional + point + spot;
}

vec3 fnk_shadeAllLightsCookTorranceGGX(QrkMaterial material, vec3 fragPos,
                                       vec3 normal, vec2 texCoords) {
  return fnk_shadeAllLightsCookTorranceGGX(material, fragPos, normal, texCoords,
                                           /*shadow=*/0.0);
}

/**
 * Calculate shading from all light sources, except ambient and emission
 * textures, using deferred data.
 * AO can be a mix of AO textures and SSAO, but the mixing should be handled by
 * the caller.
 */
vec3 fnk_shadeAllLightsCookTorranceGGXDeferred(vec3 albedo, float roughness,
                                               float metallic, vec3 fragPos,
                                               vec3 normal, float shadow) {
  vec3 directional = fnk_shadeAllDirectionalLightsCookTorranceGGXDeferred(
      albedo, roughness, metallic, fragPos, normal, shadow);
  vec3 point = fnk_shadeAllPointLightsCookTorranceGGXDeferred(
      albedo, roughness, metallic, fragPos, normal);
  vec3 spot = fnk_shadeAllSpotLightsCookTorranceGGXDeferred(
      albedo, roughness, metallic, fragPos, normal);
  return directional + point + spot;
}

vec3 fnk_shadeAllLightsCookTorranceGGXDeferred(vec3 albedo, float roughness,
                                               float metallic, vec3 fragPos,
                                               vec3 normal) {
  return fnk_shadeAllLightsCookTorranceGGXDeferred(albedo, roughness, metallic,
                                                   fragPos, normal,
                                                   /*shadow=*/0.0);
}