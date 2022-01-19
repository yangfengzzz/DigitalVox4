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
class MeshLoader {
public:
    MeshLoader() = delete;
    
    explicit MeshLoader(MTL::Device* device);
    
    void loadMesh(const char *bundlePath,
                  const char *meshFile,
                  EntityPtr entity);
    
private:
    MTL::VertexDescriptor _defaultVertexDescriptor;
    MTL::Device *_device{nullptr};
};


}

#endif // Mesh_h
