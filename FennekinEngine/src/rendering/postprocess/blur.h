#pragma once

#include "rendering/core/framebuffer.h"
#include "rendering/registers/texture_registry.h"
#include "rendering/resources/shader.h"
#include "rendering/resources/shader_primitives.h"
#include "scene/mesh_primitives.h"

class PingPongPass {
public:
    PingPongPass(int t_width, int t_height);
    explicit PingPongPass(const ImageSize t_size) : PingPongPass(t_size.width, t_size.height) {
    }
    virtual ~PingPongPass() = default;

    // Draws using the given shader multiple times, using the configured source
    // first and then ping-ponging between internal framebuffers. Passes represent
    // a single ping-pong cycle (so N passes means N*2 draw calls). The callback
    // should configure or update any needed shader uniforms, etc. After drawing,
    // you can call getOutput() to get the resulting texture.
    void multipassDraw(Texture t_source, Shader& t_shader, int t_passes, std::function<void()> t_callback,
                       TextureRegistry* t_textureRegistry = nullptr);

    Texture getOutput() {
        // The final result is always stored in the 2nd FBO.
        return m_attachments[1].asTexture();
    }

private:
    ScreenQuadMesh m_screenQuad;
    // Ping-pong buffers and attachments.
    Framebuffer m_buffers[2];
    Attachment m_attachments[2];
};

// A shader that performs a single-pass gaussian blur.
class GaussianBlurShader : public ScreenShader {
public:
    GaussianBlurShader();

    // Whether to perform a horizontal blur. If false, performs a vertical blur.
    void setHorizontal(const bool t_horizontal) {
        m_horizontal = t_horizontal;
        setBool("horizontal", m_horizontal);
    }

    [[nodiscard]] bool getHorizontal() const {
        return m_horizontal;
    }

private:
    bool m_horizontal = false;
};