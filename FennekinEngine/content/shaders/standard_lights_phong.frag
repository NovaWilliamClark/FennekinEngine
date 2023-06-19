#pragma once

#pragma fnk_include < lighting.frag>
#pragma fnk_include < phong.frag>
#pragma fnk_include < standard_lights.frag>

/** Calculate shading from all active directional lights. */
vec3 fnk_shadeAllDirectionalLightsBlinnPhong(QrkMaterial material, vec3 fragPos,
                                             vec3 normal, vec2 texCoords,
                                             float shadow, float ao) {
  vec3 result = vec3(0.0);
  for (int i = 0; i < fnk_directionalLightCount; i++) {
    result += fnk_shadeDirectionalLightBlinnPhong(
        material, fnk_directionalLights[i], fragPos, normal, texCoords, shadow,
        ao);
  }
  return result;
}

/** Calculate shading from all active directional lights using deferred data. */
vec3 fnk_shadeAllDirectionalLightsBlinnPhongDeferred(vec3 albedo, vec3 specular,
                                                     vec3 ambient,
                                                     float shininess,
                                                     vec3 fragPos, vec3 normal,
                                                     float shadow, float ao) {
  vec3 result = vec3(0.0);
  for (int i = 0; i < fnk_directionalLightCount; i++) {
    result += fnk_shadeDirectionalLightBlinnPhongDeferred(
        albedo, specular, ambient, shininess, fnk_directionalLights[i], fragPos,
        normal, shadow, ao);
  }
  return result;
}

/** Calculate shading from all active point lights. */
vec3 fnk_shadeAllPointLightsBlinnPhong(QrkMaterial material, vec3 fragPos,
                                       vec3 normal, vec2 texCoords, float ao) {
  vec3 result = vec3(0.0);
  for (int i = 0; i < fnk_pointLightCount; i++) {
    result += fnk_shadePointLightBlinnPhong(material, fnk_pointLights[i],
                                            fragPos, normal, texCoords, ao);
  }
  return result;
}

/** Calculate shading from all active point lights using deferred data. */
vec3 fnk_shadeAllPointLightsBlinnPhongDeferred(vec3 albedo, vec3 specular,
                                               vec3 ambient, float shininess,
                                               vec3 fragPos, vec3 normal,
                                               float ao) {
  vec3 result = vec3(0.0);
  for (int i = 0; i < fnk_pointLightCount; i++) {
    result += fnk_shadePointLightBlinnPhongDeferred(
        albedo, specular, ambient, shininess, fnk_pointLights[i], fragPos,
        normal, ao);
  }
  return result;
}

/** Calculate shading from all active spot lights. */
vec3 fnk_shadeAllSpotLightsBlinnPhong(QrkMaterial material, vec3 fragPos,
                                      vec3 normal, vec2 texCoords, float ao) {
  vec3 result = vec3(0.0);
  for (int i = 0; i < fnk_spotLightCount; i++) {
    result += fnk_shadeSpotLightBlinnPhong(material, fnk_spotLights[i], fragPos,
                                           normal, texCoords, ao);
  }
  return result;
}

/** Calculate shading from all active spot lights using deferred data. */
vec3 fnk_shadeAllSpotLightsDeferredBlinnPhong(vec3 albedo, vec3 specular,
                                              vec3 ambient, float shininess,
                                              vec3 fragPos, vec3 normal,
                                              float ao) {
  vec3 result = vec3(0.0);
  for (int i = 0; i < fnk_spotLightCount; i++) {
    result += fnk_shadeSpotLightBlinnPhongDeferred(albedo, specular, ambient,
                                                   shininess, fnk_spotLights[i],
                                                   fragPos, normal, ao);
  }
  return result;
}

/** Calculate shading from all light sources, except emission textures. */
vec3 fnk_shadeAllLightsBlinnPhong(QrkMaterial material, vec3 fragPos,
                                  vec3 normal, vec2 texCoords, float shadow,
                                  float ao) {
  vec3 directional = fnk_shadeAllDirectionalLightsBlinnPhong(
      material, fragPos, normal, texCoords, shadow, ao);
  vec3 point = fnk_shadeAllPointLightsBlinnPhong(material, fragPos, normal,
                                                 texCoords, ao);
  vec3 spot = fnk_shadeAllSpotLightsBlinnPhong(material, fragPos, normal,
                                               texCoords, ao);
  return directional + point + spot;
}

vec3 fnk_shadeAllLightsBlinnPhong(QrkMaterial material, vec3 fragPos,
                                  vec3 normal, vec2 texCoords) {
  return fnk_shadeAllLightsBlinnPhong(material, fragPos, normal, texCoords,
                                      /*shadow=*/0.0, /*ao=*/1.0);
}

/** Calculate shading from all light sources, except emission textures, using
 * deferred data. */
vec3 fnk_shadeAllLightsBlinnPhongDeferred(vec3 albedo, vec3 specular,
                                          vec3 ambient, float shininess,
                                          vec3 fragPos, vec3 normal,
                                          float shadow, float ao) {
  vec3 directional = fnk_shadeAllDirectionalLightsBlinnPhongDeferred(
      albedo, specular, ambient, shininess, fragPos, normal, shadow, ao);
  vec3 point = fnk_shadeAllPointLightsBlinnPhongDeferred(
      albedo, specular, ambient, shininess, fragPos, normal, ao);
  vec3 spot = fnk_shadeAllSpotLightsDeferredBlinnPhong(
      albedo, specular, ambient, shininess, fragPos, normal, ao);
  return directional + point + spot;
}

vec3 fnk_shadeAllLightsBlinnPhongDeferred(vec3 albedo, vec3 specular,
                                          vec3 ambient, float shininess,
                                          vec3 fragPos, vec3 normal) {
  return fnk_shadeAllLightsBlinnPhongDeferred(albedo, specular, ambient,
                                              shininess, fragPos, normal,
                                              /*shadow=*/0.0, /*ao=*/1.0);
}