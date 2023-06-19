#version 460 core
#pragma fnk_include < constants.glsl>
#pragma fnk_include < pbr.frag>
#pragma fnk_include < pbr_sampling.glsl>
#pragma fnk_include < random.glsl>

out vec4 fragColor;

in vec3 cubemapCoords;

uniform samplerCube fnk_environmentMap;
uniform float fnk_roughness;
uniform uint fnk_numSamples;

const float epsilon = 0.0001;

void main() {
  vec2 mip0 = textureSize(fnk_environmentMap, 0);
  // Compute the solid angle per texel. 4*PI steradians in a sphere, 6 faces of
  // a cubemap.
  float texelSolidAngle = (4.0 * PI) / (6.0 * mip0.x * mip0.y);

  // We orient the sampling hemisphere in the direction of the cubemap sample
  // vector.
  vec3 normal = normalize(cubemapCoords);
  // Simplifying assumption, we treat the sample output dir as the view dir.
  vec3 viewDir = normal;

  float totalWeight = 0.0;
  vec3 prefilteredColor = vec3(0.0);
  for (uint i = 0u; i < fnk_numSamples; ++i) {
    vec2 Xi = fnk_hammersley(i, fnk_numSamples);
    vec3 halfVector = fnk_importanceSampleGGX(normal, fnk_roughness, Xi);
    // Reverse engineer the lightDir from the sampled halfVector.
    vec3 lightDir =
        normalize(2.0 * dot(viewDir, halfVector) * halfVector - viewDir);
    float NdotL = clamp(dot(normal, lightDir), 0.0, 1.0);
    // Make sure we ignore light directions that are behind the "virtual"
    // surface.
    if (NdotL > 0.0) {
      // Choose mip level based on roughness / PDF. This avoids certain
      // "hotspot" artifacts due to the sampling method.
      // https://chetanjags.wordpress.com/2015/08/26/image-based-lighting/
      float NdotH = clamp(dot(normal, halfVector), 0.0, 1.0);
      float HdotV = clamp(dot(halfVector, viewDir), 0.0, 1.0);
      float a = fnk_roughness * fnk_roughness;
      float D = fnk_distributionGGX(NdotH, a);
      // TODO: Why add the epsilon afterwards?
      float pdf = D * NdotH / (4.0 * HdotV) + epsilon;

      float sampleSolidAngle = 1.0 / (float(fnk_numSamples) * pdf + epsilon);
      float mipLevel = fnk_roughness == 0.0
                           ? 0.0
                           : 0.5 * log2(sampleSolidAngle / texelSolidAngle);

      prefilteredColor +=
          textureLod(fnk_environmentMap, lightDir, mipLevel).rgb * NdotL;
      totalWeight += NdotL;
    }
  }
  prefilteredColor = prefilteredColor / totalWeight;
  fragColor = vec4(prefilteredColor, 1.0);
}