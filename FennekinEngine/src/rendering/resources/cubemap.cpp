#include "cubemap.hpp"

void CubemapRenderHelper::multipassDraw(Shader& shader,
                                        TextureRegistry* textureRegistry) {
  // Set projection to a 90-degree, 1:1 aspect ratio in order to render a single
  // face of the cube.
  shader.setMat4("projection", glm::perspective(glm::radians(90.0f),
                                                /*aspect=*/1.0f, 0.1f, 10.0f));

  // TODO: Why are the up vectors negative?
  glm::mat4 faceViews[] = {
      glm::lookAt(/*eye=*/glm::vec3(0.0f, 0.0f, 0.0f),
                  glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
      glm::lookAt(/*eye=*/glm::vec3(0.0f, 0.0f, 0.0f),
                  glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
      glm::lookAt(/*eye=*/glm::vec3(0.0f, 0.0f, 0.0f),
                  glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
      glm::lookAt(/*eye=*/glm::vec3(0.0f, 0.0f, 0.0f),
                  glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
      glm::lookAt(/*eye=*/glm::vec3(0.0f, 0.0f, 0.0f),
                  glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
      glm::lookAt(/*eye=*/glm::vec3(0.0f, 0.0f, 0.0f),
                  glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))};

  for (int cubemapFace = 0; cubemapFace < 6; ++cubemapFace) {
    buffer_->activate(targetMip_, cubemapFace);
    buffer_->clear();

    shader.setMat4("view", faceViews[cubemapFace]);
    room_.draw(shader, textureRegistry);
  }

  buffer_->deactivate();
}

EquirectCubemapShader::EquirectCubemapShader()
    : Shader(ShaderPath("content/shaders/builtin/cubemap.vert"),
             ShaderPath("content/shaders/builtin/equirect_cubemap.frag")) {}

EquirectCubemapConverter::EquirectCubemapConverter(int width, int height,
                                                   bool generateMips)
    : m_buffer(width, height),
      m_cubemapRenderHelper(&m_buffer),
      m_generateMips(generateMips) {
  // Optionally allocate memory for mips if requested.
  TextureParams params = {
      .filtering = generateMips ? ETextureFiltering::TRILINEAR
                                : ETextureFiltering::BILINEAR,
      .wrapMode = ETextureWrapMode::CLAMP_TO_EDGE,
      .generateMips =
          generateMips ? EMipGeneration::ALWAYS : EMipGeneration::NEVER,
  };
  m_cubemap = m_buffer.attachTexture(EBufferType::COLOR_CUBEMAP_HDR, params);
}

void EquirectCubemapConverter::multipassDraw(Texture t_source) {
  // Set up the source.
  t_source.bindToUnit(0, ETextureBindType::TEXTURE_2D);
  m_equirectCubemapShader.setInt("fnk_equirectMap", 0);

  m_cubemapRenderHelper.multipassDraw(m_equirectCubemapShader);

  if (m_generateMips) {
    // Generate mips after having rendered to the cubemap.
    m_cubemap.asTexture().generateMips();
  }
}

unsigned int EquirectCubemapConverter::bindTexture(unsigned int t_nextTextureUnit,
                                                   Shader& t_shader) {
  m_cubemap.asTexture().bindToUnit(t_nextTextureUnit, ETextureBindType::CUBEMAP);
  // Bind sampler uniforms.
  t_shader.setInt("fnk_cubemap", t_nextTextureUnit);

  return t_nextTextureUnit + 1;
}
