#version 460 core
#pragma fnk_include < core.glsl>
#pragma fnk_include < lighting.frag>
#pragma fnk_include < normals.frag>

// Deferred geometry pass fragment shader.

in VS_OUT {
  vec2 texCoords;
  vec3 fragPos_viewSpace;
  vec3 fragNormal_viewSpace;
  mat3 fragTBN_viewSpace;  // Transforms from tangent frame to view frame.
}
fs_in;

layout(location = 0) out vec4 gPositionAO;
layout(location = 1) out vec4 gNormalRoughness;
layout(location = 2) out vec4 gAlbedoMetallic;
layout(location = 3) out vec3 gEmission;

uniform FnkMaterial material;

void main() {
  // Fill the G-Buffer.

  // Map the fragment position and AO.
  gPositionAO.rgb = fs_in.fragPos_viewSpace;
  gPositionAO.a = fnk_extractAmbientOcclusion(material, fs_in.texCoords);
  // Lookup normal and map from tangent space to view space. Falls back to
  // vertex normal otherwise.
  gNormalRoughness.rgb =
      fnk_getNormal(material, fs_in.texCoords, fs_in.fragTBN_viewSpace,
                    fs_in.fragNormal_viewSpace);
  gNormalRoughness.a = fnk_extractRoughness(material, fs_in.texCoords);

  gAlbedoMetallic.rgb = fnk_extractAlbedo(material, fs_in.texCoords);
  gAlbedoMetallic.a = fnk_extractMetallic(material, fs_in.texCoords);

  // We currently don't store anything in the alpha channel.
  gEmission = fnk_extractEmission(material, fs_in.texCoords);
}
