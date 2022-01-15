//
//  primitive_mesh.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/14.
//

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
