//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "shadow_debug_material.h"

namespace vox {
ShadowDebugMaterial::ShadowDebugMaterial():
BaseMaterial(Shader::create("shadow_debug", "vertex_blinn_phong", "fragment_cascade_shadow_debugger")) {
}

}
