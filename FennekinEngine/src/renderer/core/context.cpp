#include "context.hpp"

/**
 * @brief Enables the depth test.
 *
 * This function enables the depth test in OpenGL and sets the depth test flag to true.
 */
void Context::enableDepthTest() {
    glEnable(GL_DEPTH_TEST);
    m_depthTestEnabled = true;
}

/**
 * @brief Disables the depth test.
 *
 * This function disables the depth test in OpenGL and sets the depth test flag to false.
 */
void Context::disableDepthTest() {
    glDisable(GL_DEPTH_TEST);
    m_depthTestEnabled = false;
}

/**
 * @brief Enables the stencil test.
 *
 * This function enables the stencil test in OpenGL, sets the stencil operation, and sets the stencil test flag to true.
 */
void Context::enableStencilTest() {
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    m_stencilTestEnabled = true;
}

/**
 * @brief Disables the stencil test.
 *
 * This function disables the stencil test in OpenGL and sets the stencil test flag to false.
 */
void Context::disableStencilTest() {
    glDisable(GL_STENCIL_TEST);
    m_stencilTestEnabled = false;
}

/**
 * @brief Enables stencil updates.
 *
 * This function sets the stencil mask to allow updates.
 */
void Context::enableStencilUpdates() {
    glStencilMask(0xFF);
}

/**
 * @brief Disables stencil updates.
 *
 * This function sets the stencil mask to prevent updates.
 */
void Context::disableStencilUpdates() {
    glStencilMask(0x00);
}

/**
 * @brief Sets the stencil function to always draw.
 *
 * This function sets the stencil function to GL_ALWAYS, which means the stencil test always passes.
 */
void Context::stencilAlwaysDraw() {
    setStencilFunc(GL_ALWAYS);
}

/**
 * @brief Sets the stencil function to draw when matching.
 *
 * This function sets the stencil function to GL_EQUAL, which means the stencil test passes when the reference and the stored stencil value are equal.
 */
void Context::stencilDrawWhenMatching() {
    setStencilFunc(GL_EQUAL);
}

/**
 * @brief Sets the stencil function to draw when not matching.
 *
 * This function sets the stencil function to GL_NOTEQUAL, which means the stencil test passes when the reference and the stored stencil value are not equal.
 */
void Context::stencilDrawWhenNotMatching() {
    setStencilFunc(GL_NOTEQUAL);
}

/**
 * @brief Sets the stencil function.
 *
 * @param t_func The stencil function to set.
 *
 * This function sets the stencil function to the given value, with a reference value of 1 and a mask of 0xFF.
 */
void Context::setStencilFunc(const GLenum t_func) {
    glStencilFunc(t_func, 1, 0xFF);
}

/**
 * @brief Enables alpha blending.
 *
 * This function enables alpha blending in OpenGL, sets the blend function, and sets the blend equation.
 */
void Context::enableAlphaBlending() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
}

/**
 * @brief Disables alpha blending.
 *
 * This function disables alpha blending in OpenGL.
 */
void Context::disableAlphaBlending() {
    glDisable(GL_BLEND);
}

/**
 * @brief Enables face culling.
 *
 * This function enables face culling in OpenGL.
 */
void Context::enableFaceCull() {
    glEnable(GL_CULL_FACE);
}

/**
 * @brief Disables face culling.
 *
 * This function disables face culling in OpenGL.
 */
void Context::disableFaceCull() {
    glDisable(GL_CULL_FACE);
}

/**
 * @brief Enables wireframe mode.
 *
 * This function sets the polygon mode to GL_LINE, which results in all polygons being rendered as wireframes.
 */
void Context::enableWireframe() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

/**
 * @brief Disables wireframe mode.
 *
 * This function sets the polygon mode to GL_FILL, which results in all polygons being rendered as filled.
 */
void Context::disableWireframe() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

/**
 * @brief Enables seamless cubemap texturing.
 *
 * This function enables seamless cubemap texturing in OpenGL.
 */
void Context::enableSeamlessCubemap() {
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

/**
 * @brief Disables seamless cubemap texturing.
 *
 * This function disables seamless cubemap texturing in OpenGL.
 */
void Context::disableSeamlessCubemap() {
    glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

/**
 * @brief Sets face culling to cull front faces.
 *
 * This function sets the face culling mode in OpenGL to cull front faces.
 */
void Context::cullFrontFaces() {
    glCullFace(GL_FRONT);
}

/**
 * @brief Sets face culling to cull back faces.
 *
 * This function sets the face culling mode in OpenGL to cull back faces.
 */
void Context::cullBackFaces() {
    glCullFace(GL_BACK);
}

/**
 * @brief Enables multi sampling.
 *
 * This function enables multi sampling. in OpenGL.
 */
void Context::enableMultiSampling() {
    glEnable(GL_MULTISAMPLE);
}

/**
 * @brief disables multi sampling.
 *
 * This function disables multi sampling. in OpenGL.
 */
void Context::disableMultiSampling() {
    glDisable(GL_MULTISAMPLE);
}
