#pragma once

#pragma fnk_include < constants.glsl>
#pragma fnk_include < lighting.frag>

/**
 * Calculate the deferred Blinn-Phong shading model with ambient, diffuse, and
 * specular components, along with direct material colors. Does not include
 * attenuation.
 */
vec3 fnk_shadeBlinnPhongDeferred(vec3 albedo, vec3 specular, vec3 ambient,
                                 float shininess, vec3 lightDiffuse,
                                 vec3 lightSpecular, vec3 lightDir,
                                 vec3 viewDir, vec3 normal, float intensity,
                                 float shadow, float ao) {
  vec3 result = vec3(0.0);
  float shadowMultiplier = 1.0 - shadow;

  // Ambient and diffuse components.
  float diffuseIntensity = max(dot(normal, lightDir), 0.0);
  // Ambient component. Don't include shadow calculation here, since it
  // shouldn't affect ambient light, but do include AO.
  // TODO: Ambient should not be here, as this is a per-light term. Removing it
  // will allow also extracting (spotlight)intensity, shadow, and ambient
  // occlusion params.
  result += ambient * albedo * ao;

  // Diffuse component.
  result +=
      lightDiffuse * diffuseIntensity * albedo * intensity * shadowMultiplier;

  // Specular component.
  vec3 halfwayDir = normalize(lightDir + viewDir);
  float specularIntensity = pow(max(dot(normal, halfwayDir), 0.0), shininess);
  result += lightSpecular * specularIntensity * specular * intensity *
            shadowMultiplier;

  return result;
}

/**
 * Calculate the Blinn-Phong shading model with ambient, diffuse, and specular
 * components. Does not include attenuation.
 */
vec3 fnk_shadeBlinnPhong(FnkMaterial material, vec3 lightDiffuse,
                         vec3 lightSpecular, vec3 lightDir, vec3 viewDir,
                         vec3 normal, vec2 texCoords, float intensity,
                         float shadow, float ao) {
  vec3 albedo = fnk_extractAlbedo(material, texCoords);
  vec3 specular = fnk_extractSpecular(material, texCoords);
  // Use the deferred calculations, but do it directly, so it's not deferred.
  // :)
  return fnk_shadeBlinnPhongDeferred(
      albedo, specular, material.ambient, material.shininess, lightDiffuse,
      lightSpecular, lightDir, viewDir, normal, intensity, shadow, ao);
}

/** Calculate shading for a directional light source using deferred data. */
vec3 fnk_shadeDirectionalLightBlinnPhongDeferred(vec3 albedo, vec3 specular,
                                                 vec3 ambient, float shininess,
                                                 FnkDirectionalLight light,
                                                 vec3 fragPos, vec3 normal,
                                                 float shadow, float ao) {
  vec3 lightDir = normalize(-light.direction);
  vec3 viewDir = normalize(-fragPos);

  return fnk_shadeBlinnPhongDeferred(
      albedo, specular, ambient, shininess, light.diffuse, light.specular,
      lightDir, viewDir, normal, /*intensity=*/1.0, shadow, ao);
}

/** Calculate shading for a directional light source. */
vec3 fnk_shadeDirectionalLightBlinnPhong(FnkMaterial material,
                                         FnkDirectionalLight light,
                                         vec3 fragPos, vec3 normal,
                                         vec2 texCoords, float shadow,
                                         float ao) {
  vec3 albedo = fnk_extractAlbedo(material, texCoords);
  vec3 specular = fnk_extractSpecular(material, texCoords);
  return fnk_shadeDirectionalLightBlinnPhongDeferred(
      albedo, specular, material.ambient, material.shininess, light, fragPos,
      normal, shadow, ao);
}

/** Calculate shading for a point light source using deferred data. */
vec3 fnk_shadePointLightBlinnPhongDeferred(vec3 albedo, vec3 specular,
                                           vec3 ambient, float shininess,
                                           FnkPointLight light, vec3 fragPos,
                                           vec3 normal, float ao) {
  vec3 lightDir = normalize(light.position - fragPos);
  vec3 viewDir = normalize(-fragPos);

  // Calculate attenuation from light source.
  float lightDist = length(light.position - fragPos);
  float attenuation = fnk_calcAttenuation(light.attenuation, lightDist);

  vec3 result = fnk_shadeBlinnPhongDeferred(
      albedo, specular, ambient, shininess, light.diffuse, light.specular,
      lightDir, viewDir, normal, /*intensity=*/1.0, /*shadow=*/0.0, ao);
  // Apply attenuation.
  return result * attenuation;
}

/** Calculate shading for a point light source. */
vec3 fnk_shadePointLightBlinnPhong(FnkMaterial material, FnkPointLight light,
                                   vec3 fragPos, vec3 normal, vec2 texCoords,
                                   float ao) {
  vec3 albedo = fnk_extractAlbedo(material, texCoords);
  vec3 specular = fnk_extractSpecular(material, texCoords);
  return fnk_shadePointLightBlinnPhongDeferred(
      albedo, specular, material.ambient, material.shininess, light, fragPos,
      normal, ao);
}

/** Calculate shading for a spot light source using deferred data. */
vec3 fnk_shadeSpotLightBlinnPhongDeferred(vec3 albedo, vec3 specular,
                                          vec3 ambient, float shininess,
                                          FnkSpotLight light, vec3 fragPos,
                                          vec3 normal, float ao) {
  vec3 lightDir = normalize(light.position - fragPos);
  vec3 viewDir = normalize(-fragPos);

  // Calculate attenuation from light source.
  float lightDist = length(light.position - fragPos);
  float attenuation = fnk_calcAttenuation(light.attenuation, lightDist);

  float intensity = fnk_calcSpotLightIntensity(light, lightDir);

  vec3 result = fnk_shadeBlinnPhongDeferred(
      albedo, specular, ambient, shininess, light.diffuse, light.specular,
      lightDir, viewDir, normal, intensity, /*shadow=*/0.0, ao);
  // Apply attenuation.
  return result * attenuation;
}

/** Calculate shading for a spot light source. */
vec3 fnk_shadeSpotLightBlinnPhong(FnkMaterial material, FnkSpotLight light,
                                  vec3 fragPos, vec3 normal, vec2 texCoords,
                                  float ao) {
  vec3 albedo = fnk_extractAlbedo(material, texCoords);
  vec3 specular = fnk_extractSpecular(material, texCoords);
  return fnk_shadeSpotLightBlinnPhongDeferred(
      albedo, specular, material.ambient, material.shininess, light, fragPos,
      normal, ao);
}