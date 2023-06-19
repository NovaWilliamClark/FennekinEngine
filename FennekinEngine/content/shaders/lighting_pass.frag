#version 460 core
#pragma fnk_include < core.glsl>
#pragma fnk_include < standard_lights_phong.frag>
#pragma fnk_include < standard_lights_pbr.frag>
#pragma fnk_include < depth.frag>
#pragma fnk_include < tone_mapping.frag>

// A fragment shader for rendering models.

in vec2 texCoords;

out vec4 fragColor;

uniform sampler2D gPositionAO;
uniform sampler2D gNormalRoughness;
uniform sampler2D gAlbedoMetallic;
uniform sampler2D gEmission;

uniform bool shadowMapping;
uniform bool ssao;
uniform sampler2D fnk_ssao;

uniform vec3 ambient;
uniform float shininess;
uniform float emissionIntensity;
uniform QrkAttenuation emissionAttenuation;

uniform int lightingModel;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightViewProjection;
uniform sampler2D shadowMap;
uniform float shadowBiasMin;
uniform float shadowBiasMax;
uniform samplerCube fnk_irradianceMap;
uniform bool useIBL;
uniform samplerCube fnk_ggxPrefilteredEnvMap;
uniform float fnk_ggxPrefilteredEnvMapMaxLOD;
uniform sampler2D fnk_ggxIntegrationMap;

void main() {
  // Extract G-Buffer for PBR rendering.
  vec3 fragPos_viewSpace = texture(gPositionAO, texCoords).rgb;
  float fragAO = texture(gPositionAO, texCoords).a;
  vec3 fragNormal_viewSpace = texture(gNormalRoughness, texCoords).rgb;
  float fragRoughness = texture(gNormalRoughness, texCoords).a;
  vec3 fragAlbedo = texture(gAlbedoMetallic, texCoords).rgb;
  float fragMetallic = texture(gAlbedoMetallic, texCoords).a;
  vec3 fragEmission = texture(gEmission, texCoords).rgb;

  vec3 color;

  // Shadow mapping. Currently only supported for one dir light.
  float shadow = 0.0;
  if (shadowMapping) {
    float shadowBias =
        fnk_shadowBias(shadowBiasMin, shadowBiasMax, fragNormal_viewSpace,
                       fnk_directionalLights[0].direction);
    // Since we're in view space, we have to un-project to world space in order
    // to get to the light's view.
    vec4 fragPos_worldSpace = inverse(view) * vec4(fragPos_viewSpace, 1.0);
    vec4 fragPos_lightSpace = lightViewProjection * fragPos_worldSpace;
    shadow = fnk_shadow(shadowMap, fragPos_lightSpace, shadowBias);
  }

  // Ambient occlusion.
  float ao = fragAO;
  if (ssao) {
    // Add SSAO and combined with texture based ambient occlusion from the
    // G-buffer.
    ao *= texture(fnk_ssao, texCoords).r;
  }

  // Shade with normal lights.
  if (lightingModel == 0) {
    // Phong.
    color = fnk_shadeAllLightsBlinnPhongDeferred(
        fragAlbedo, /*specular=*/vec3(fragMetallic), ambient, shininess,
        fragPos_viewSpace, fragNormal_viewSpace, shadow, ao);
  } else if (lightingModel == 1) {
    // GGX.
    color = fnk_shadeAllLightsCookTorranceGGXDeferred(
        fragAlbedo, fragRoughness, fragMetallic, fragPos_viewSpace,
        fragNormal_viewSpace, shadow);
  } else {
    // Invalid lighting model (pink to signal!).
    color = vec3(1.0, 0.0, 0.5);
  }

      // Add ambient term.
    if (useIBL) {
      // Need to sample from cubemaps via worlspace vectors.
      vec3 fragNormal_worldSpace = mat3(transpose(view)) * fragNormal_viewSpace;
      vec3 viewDir_worldSpace =
          mat3(inverse(view)) * normalize(-fragPos_viewSpace);
      vec3 reflectionDir_worldSpace =
          reflect(-viewDir_worldSpace, fragNormal_worldSpace);

      // Sample textures needed for diffuse and specular IBL terms.
      vec3 fragIrradiance =
          texture(fnk_irradianceMap, normalize(fragNormal_worldSpace)).rgb;
      vec3 prefilteredEnvColor = fnk_samplePrefilteredEnvMap(
          viewDir_worldSpace, fragNormal_worldSpace, fragRoughness,
          fnk_ggxPrefilteredEnvMap, fnk_ggxPrefilteredEnvMapMaxLOD);
      vec2 envBRDF =
          fnk_sampleBrdfLUT(viewDir_worldSpace, fragNormal_worldSpace,
                            fragRoughness, fnk_ggxIntegrationMap);

      color += fnk_shadeAmbientIBLDeferred(
          fragAlbedo, fragIrradiance, prefilteredEnvColor, envBRDF,
          fragRoughness, fragMetallic, ao, viewDir_worldSpace,
          fragNormal_worldSpace);
    } else {
      color += fnk_shadeAmbientDeferred(fragAlbedo, ambient, ao);
    }

  // Add emissions.
  color += emissionIntensity * fnk_shadeEmissionDeferred(fragEmission,
                                                         fragPos_viewSpace,
                                                         emissionAttenuation);

  fragColor = vec4(color, 1.0);
}
