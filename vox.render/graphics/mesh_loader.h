//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef Mesh_h
#define Mesh_h

#include "entity.h"
#include "scene.h"
#include "core/cpp_mtl.h"

namespace vox {
void newMeshesFromBundlePath(const char *bundlePath,
                             const char *meshFile,
                             MTL::Device &device,
                             EntityPtr entity,
                             const MTL::VertexDescriptor &vertexDescriptor);
}

#endif // Mesh_h
