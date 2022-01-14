/*
 See LICENSE folder for this sample’s licensing information.
 
 Abstract:
 Header for mesh and submesh objects used for managing models
 */
#ifndef Mesh_h
#define Mesh_h

#include "mesh.h"

namespace vox {
std::vector<Mesh> *newMeshesFromBundlePath(const char *bundlePath,
                                           const char *meshFile,
                                           MTL::Device &device,
                                           const MTL::VertexDescriptor &vertexDescriptor,
                                           CFErrorRef *error);


Mesh makeSphereMesh(MTL::Device &device,
                    const MTL::VertexDescriptor &vertexDescriptor,
                    int radialSegments, int verticalSegments, float radius);

Mesh makeIcosahedronMesn(MTL::Device &device,
                         const MTL::VertexDescriptor &vertexDescriptor,
                         float radius);

}

#endif // Mesh_h
