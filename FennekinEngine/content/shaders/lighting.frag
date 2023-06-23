#pragma once

#pragma fnk_include < gamma.frag>
#pragma fnk_include < normals.frag>

/** Core lighting structs and functions. */

struct FnkAttenuation {
  float constant;
  float linear;
  float quadratic;
};

#ifndef fnk_MAX_DIFFUSE_TEXTURES
#define fnk_MAX_DIFFUSE_TEXTURES 1
#endif

#ifndef fnk_MAX_SPECULAR_TEXTURES
#define fnk_MAX_SPECULAR_TEXTURES 1
#endif

#ifndef fnk_MAX_ROUGHNESS_TEXTURES
#define fnk_MAX_ROUGHNESS_TEXTURES 1
#endif

#ifndef fnk_MAX_METALLIC_TEXTURES
#define fnk_MAX_METALLIC_TEXTURES 1
#endif

#ifndef fnk_MAX_AO_TEXTURES
#define fnk_MAX_AO_TEXTURES 1
#endif

#ifndef fnk_MAX_EMISSION_TEXTURES
#define fnk_MAX_EMISSION_TEXTURES 1
#endif

struct FnkMaterial {
  // Material maps. Standard lighting logic only uses a single map, but
  // multiple maps are available in case user code wants to do something fancy.

  // TODO: Consider splitting out phong / PBR material properties.
  sampler2D diffuseMaps[fnk_MAX_DIFFUSE_TEXTURES];
  int diffuseCount;

  sampler2D specularMaps[fnk_MAX_SPECULAR_TEXTURES];
  int specularCount;

  sampler2D roughnessMaps[fnk_MAX_ROUGHNESS_TEXTURES];
  bool roughnessIsPacked[fnk_MAX_ROUGHNESS_TEXTURES];
  int roughnessCount;

  sampler2D metallicMaps[fnk_MAX_METALLIC_TEXTURES];
  bool metallicIsPacked[fnk_MAX_METALLIC_TEXTURES];
  int metallicCount;

  sampler2D aoMaps[fnk_MAX_AO_TEXTURES];
  bool aoIsPacked[fnk_MAX_AO_TEXTURES];
  int aoCount;

  sampler2D emissionMaps[fnk_MAX_EMISSION_TEXTURES];
  int emissionCount;

  sampler2D normalMap;
  bool hasNormalMap;

  // Ambient light factor.
  vec3 ambient;

  float shininess;

  // TODO: Add emissionFactor.
  FnkAttenuation emissionAttenuation;
};

struct FnkDirectionalLight {
  vec3 direction;

  vec3 diffuse;
  vec3 specular;
};

struct FnkPointLight {
  vec3 position;

  vec3 diffuse;
  vec3 specular;

  FnkAttenuation attenuation;
};

struct FnkSpotLight {
  vec3 position;
  vec3 direction;
  float innerAngle;
  float outerAngle;

  vec3 diffuse;
  vec3 specular;

  FnkAttenuation attenuation;
};

/** Extracts albedo from the material. */
vec3 fnk_extractAlbedo(FnkMaterial material, vec2 texCoords) {
  vec3 albedo = vec3(0.0);
  if (material.diffuseCount > 0) {
    albedo = texture(material.diffuseMaps[0], texCoords).rgb;
  }
  return albedo;
}

/** Extracts specular from the material. */
vec3 fnk_extractSpecular(FnkMaterial material, vec2 texCoords) {
  // In the absence of a specular map, we just calculate a half specular
  // component.
  vec3 specular = vec3(0.5);
  if (material.specularCount > 0) {
    // We only need a single channel. Sometimes we treat metallic maps as
    // specular maps, so extract from the blue channel in case the metallic map
    // is part of a packed roughness/metallic texture.
    specular = vec3(texture(material.specularMaps[0], texCoords).b);
  }
  return specular;
}

/** Extracts roughness from the material. */
float fnk_extractRoughness(FnkMaterial material, vec2 texCoords) {
  float roughness = 0.5;
  if (material.roughnessCount > 0) {
    if (material.roughnessIsPacked[0]) {
      // Part of a packed texture. Traditionally, roughness is the green
      // channel.
      roughness = texture(material.roughnessMaps[0], texCoords).g;
    } else {
      // Separate texture.
      roughness = texture(material.roughnessMaps[0], texCoords).r;
    }
  }
  return roughness;
}

/** Extracts metallic from the material. */
float fnk_extractMetallic(FnkMaterial material, vec2 texCoords) {
  float metallic = 0.0;
  if (material.metallicCount > 0) {
    if (material.metallicIsPacked[0]) {
      // Part of a packed texture. Traditionally, metallic is the blue channel.
      metallic = texture(material.metallicMaps[0], texCoords).b;
    } else {
      // Separate texture.
      metallic = texture(material.metallicMaps[0], texCoords).r;
    }
  }
  return metallic;
}

/**
 * Extracts the ambient occlusion from the material. This is a value that can be
 * directly multiplied with lighting, with 0 == fully occluded, and 1 == not at
 * all occluded.
 */
float fnk_extractAmbientOcclusion(FnkMaterial material, vec2 texCoords) {
  float ao = 1.0;
  if (material.aoCount > 0) {
    // We could check `material.aoIsPacked[0]` here, but even in packed textures
    // we assume that AO is in the red channel, so we can avoid that check. :)
    ao = texture(material.aoMaps[0], texCoords).r;
  }
  return ao;
}

/** Extracts emission from the material. */
vec3 fnk_extractEmission(FnkMaterial material, vec2 texCoords) {
  vec3 emission = vec3(0.0);
  if (material.emissionCount > 0) {
    emission = texture(material.emissionMaps[0], texCoords).rgb;
  }
  return emission;
}

/**
 * Calculate a material's final alpha based on its set of diffuse textures.
 */
float fnk_materialAlpha(FnkMaterial material, vec2 texCoords) {
  float sum = 0.0;
  for (int i = 0; i < material.diffuseCount; i++) {
    sum += texture(material.diffuseMaps[i], texCoords).a;
  }
  return min(sum, 1.0);
}

/**
 * Calculate attenuation based on fragment distance from a light source.
 * Returns a multipler that can be used in shading.
 */
float fnk_calcAttenuation(FnkAttenuation attenuation, float fragDist) {
  // Avoid dividing by zero.
  return 1.0 / max(attenuation.constant + attenuation.linear * fragDist +
                       attenuation.quadratic * (fragDist * fragDist),
                   1e-4);
}

/**
 * Calculate the directional intensity for a spotlight given the direction from
 * a fragment.
 */
float fnk_calcSpotLightIntensity(FnkSpotLight light, vec3 lightDir) {
  // Calculate cosine of the angle between the spotlight's direction vector and
  // the direction from the light to the current fragment.
  float theta = dot(lightDir, normalize(-light.direction));

  // Calculate the intensity based on fragment position, having zero intensity
  // when it falls outside the cone, partial intensity when it lies between
  // innerAngle and outerAngle, and full intensity when it falls in the cone.
  float innerAngleCosine = cos(light.innerAngle);
  float outerAngleCosine = cos(light.outerAngle);
  float epsilon = innerAngleCosine - outerAngleCosine;
  // Things outside the spotlight will have 0 intensity.
  return clamp((theta - outerAngleCosine) / epsilon, 0.0, 1.0);
}

/** ============================ Ambient ============================ **/

/** Calculate the ambient shading component. */
vec3 fnk_shadeAmbientDeferred(vec3 albedo, vec3 ambient, float ao) {
  return albedo * ambient * ao;
}

/** Calculate shading for ambient component based on the given material. */
vec3 fnk_shadeAmbient(FnkMaterial material, vec3 albedo, vec2 texCoords) {
  float ao = fnk_extractAmbientOcclusion(material, texCoords);
  return fnk_shadeAmbientDeferred(albedo, material.ambient, ao);
}

/** ============================ Emission ============================ **/

/** Calculate deferred shading for emission based on an emission color. */
vec3 fnk_shadeEmissionDeferred(vec3 emissionColor, vec3 fragPos_viewSpace,
                               FnkAttenuation emissionAttenuation) {
  // Calculate emission attenuation towards camera.
  float fragDist = length(fragPos_viewSpace);
  float attenuation = fnk_calcAttenuation(emissionAttenuation, fragDist);
  // Emission component.
  return emissionColor * attenuation;
}

/** Calculate shading for emission textures on the given material. */
vec3 fnk_shadeEmission(FnkMaterial material, vec3 fragPos_viewSpace,
                       vec2 texCoords) {
  vec3 emission = fnk_extractEmission(material, texCoords);
  return fnk_shadeEmissionDeferred(emission, fragPos_viewSpace,
                                   material.emissionAttenuation);
}

/** ============================ Normals ============================ **/

/**
 * Looks up a normal from the material, using the provided TBN matrix to
 * convert from tangent space to the target space, or returns a vertex normal
 * if no normal map is present.
 */
vec3 fnk_getNormal(FnkMaterial material, vec2 texCoords, mat3 TBN,
                   vec3 vertexNormal) {
  if (material.hasNormalMap) {
    return normalize(TBN * fnk_sampleNormalMap(material.normalMap, texCoords));
  } else {
    return normalize(vertexNormal);
  }
}

/** ============================ Shadows ============================ **/

/** Calculate a shadow bias based on the surface normal and light direction. */
float fnk_shadowBias(float minBias, float maxBias, vec3 normal, vec3 lightDir) {
  return max(maxBias * (1.0 - dot(normal, lightDir)), minBias);
}

/** Sample from a shadow map using 9-texel percentage-closer filtering. */
float fnk_shadowSamplePCF(sampler2D shadowMap, vec2 shadowTexCoords,
                          float currentDepth, float bias) {
  float shadow = 0.0;
  vec2 texelOffset = 1.0 / textureSize(shadowMap, /*mip=*/0);
  for (int x = -1; x <= 1; x++) {
    for (int y = -1; y <= 1; y++) {
      float pcfDepth =
          texture(shadowMap, shadowTexCoords + vec2(x, y) * texelOffset).r;
      // Check whether in shadow.
      shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
    }
  }
  return shadow / 9.0;
}

/**
 * Calculate whether the given fragment is in shadow.
 * Returns 1.0 if in shadow, 0.0 if not.
 */
float fnk_shadow(sampler2D shadowMap, vec4 fragPosLightSpace, float bias) {
  // Perform perspective divide.
  vec3 projectedPos = fragPosLightSpace.xyz / fragPosLightSpace.w;
  // Shift to the range 0..1 so that we can compare with depth.
  projectedPos = projectedPos * 0.5 + 0.5;
  // Check for out-of-frustum.
  if (projectedPos.z > 1.0) {
    // Assume not in shadow.
    return 0.0;
  }
  vec2 shadowTexCoords = projectedPos.xy;
  float currentDepth = projectedPos.z;
  return fnk_shadowSamplePCF(shadowMap, shadowTexCoords, currentDepth, bias);
}