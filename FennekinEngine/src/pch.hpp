#pragma once

#include "core/core.hpp"
#include "core/window.hpp"

#include "core/debug/debug.hpp"
#include "core/debug/exceptions.hpp"

#include "rendering/core/framebuffer.hpp"
#include "rendering/core/gBuffer.hpp"
#include "rendering/core/vertex_array.hpp"

#include "rendering/postprocess/bloom.hpp"
#include "rendering/postprocess/blur.hpp"
#include "rendering/postprocess/fxaa.hpp"
#include "rendering/postprocess/ibl.hpp"
#include "rendering/postprocess/ssao.hpp"

#include "rendering/registers/texture_registry.hpp"

#include "rendering/resources/cubemap.hpp"
#include "rendering/resources/shader.hpp"
#include "rendering/resources/shader_defs.hpp"
#include "rendering/resources/shader_primitives.hpp"
#include "rendering/resources/texture.hpp"
#include "rendering/resources/texture_map.hpp"

#include "rendering/resources/loaders/shader_compiler.hpp"
#include "rendering/resources/loaders/shader_loader.hpp"

#include "scene/camera.hpp"
#include "scene/mesh.hpp"
#include "scene/mesh_primitives.hpp"
#include "scene/model.hpp"

#include "scene/lighting/light.hpp"
#include "scene/lighting/shadows.hpp"
