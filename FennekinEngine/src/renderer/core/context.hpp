#pragma once

#include <GL/glew.h>

class Context {
public:
	void enableDepthTest();

	void disableDepthTest();

	void enableStencilTest();

	void disableStencilTest();

	void enableStencilUpdates();
	void disableStencilUpdates();

	void stencilAlwaysDraw();
	void stencilDrawWhenMatching();
	void stencilDrawWhenNotMatching();

	void setStencilFunc(const GLenum t_func);

	void enableAlphaBlending();
	void disableAlphaBlending();

	void enableFaceCull();
	void disableFaceCull();

	void enableWireframe();
	void disableWireframe();

	void enableSeamlessCubemap();
	void disableSeamlessCubemap();

	void cullFrontFaces();
	void cullBackFaces();

	void enableMultiSampling();

	void disableMultiSampling();

private:
	bool m_depthTestEnabled{};
	bool m_stencilTestEnabled{};
};
