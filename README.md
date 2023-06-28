# Fennikin Engine - William Clark

An OpenGL model renderer. WIP currently learning about shaders and 3D rendering techniques

## Features
- Core
  - Shader loader & include system
  - Model loading via Assimp
  - Light, uniforms, and texture registry
- Rendering
  - Deferred shading
  - Blinn-Phong lighting model
  - PBR lighting model (Cook-Torrance GGX)
  - Shadow mapping
  - Normal mapping
  - Runtime IBL with reflection probe prefiltering
  - SSAO (Screen Space Ambient Occlusion)
- Post-processing
  - HDR support
  - Bloom
  - Tone mapping
  - Gamma correction
  - FXAA
- Model viewer
  - Camera system with fly and orbit controls
- Debugging
  - ImGui debug menu


## Test
![Alt Text](./Docs/PBR_Model_Diffuse_Lighting_Test.gif)