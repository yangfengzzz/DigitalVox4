//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef primitive_mesh_hpp
#define primitive_mesh_hpp

#include "mesh.h"

namespace vox {
/**
 * Used to generate common primitive meshes.
 */
class PrimitiveMesh {
public:
    static MeshPtr makeSphereMesh(MTL::Device &device,
                                  const MTL::VertexDescriptor &vertexDescriptor,
                                  int radialSegments, int verticalSegments, float radius);
    
    static MeshPtr makeIcosahedronMesn(MTL::Device &device,
                                       const MTL::VertexDescriptor &vertexDescriptor,
                                       float radius);
};

}

#endif /* primitive_mesh_hpp */
