#pragma once

#include "core/core.h"
#include "core/window.h"

#include "core/debug/debug.h"
#include "core/debug/exceptions.h"

#include "rendering/core/framebuffer.h"
#include "rendering/core/gBuffer.h"
#include "rendering/core/vertex_array.h"

#include "rendering/postprocess/bloom.h"
#include "rendering/postprocess/blur.h"
#include "rendering/postprocess/fxaa.h"
#include "rendering/postprocess/ibl.h"
#include "rendering/postprocess/ssao.h"

#include "rendering/registers/texture_registry.h"

#include "rendering/resources/cubemap.h"
#include "rendering/resources/shader.h"
#include "rendering/resources/shader_defs.h"
#include "rendering/resources/shader_primitives.h"
#include "rendering/resources/texture.h"
#include "rendering/resources/texture_map.h"

#include "rendering/resources/loaders/shader_compiler.h"
#include "rendering/resources/loaders/shader_loader.h"

#include "scene/camera.h"
#include "scene/mesh.h"
#include "scene/mesh_primitives.h"
#include "scene/model.h"

#include "scene/lighting/light.h"
#include "scene/lighting/shadows.h"
