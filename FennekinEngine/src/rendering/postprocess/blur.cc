#include "blur.h"

PingPongPass::PingPongPass(int t_width, int t_height) : m_buffers{Framebuffer(t_width, t_height), Framebuffer(t_width, t_height)} {
    m_attachments[0] = m_buffers[0].attachTexture(EBufferType::COLOR_HDR_ALPHA);
    m_attachments[1] = m_buffers[1].attachTexture(EBufferType::COLOR_HDR_ALPHA);
}

void PingPongPass::multipassDraw(Texture t_source, Shader& t_shader, int t_passes, std::function<void()> t_callback,
                                 TextureRegistry* t_textureRegistry) {
    // For the very first iteration, we render from the source to buffer0.
    Framebuffer* currentFb = &m_buffers[0];
    Texture currentTexture = t_source;

    const int totalIterations = t_passes * 2;
    for (int i = 0; i < totalIterations; i++) {
        currentFb->activate();

        t_callback();
        m_screenQuad.setTexture(currentTexture);
        m_screenQuad.draw(t_shader, t_textureRegistry);

        // For every subsequent iteration, we switch our target buffer, and then
        // render from the opposing attachment.
        const int nextFbIdx = (i + 1) % 2;
        currentFb = &m_buffers[nextFbIdx];
        currentTexture = m_attachments[nextFbIdx == 0 ? 1 : 0].asTexture();

        currentFb->deactivate();
    }
}

GaussianBlurShader::GaussianBlurShader() : ScreenShader(ShaderPath("content/shaders/builtin/gaussian_blur.frag")) {
}