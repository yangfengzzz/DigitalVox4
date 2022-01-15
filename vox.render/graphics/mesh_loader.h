/*
 See LICENSE folder for this sample’s licensing information.
 
 Abstract:
 Header for mesh and submesh objects used for managing models
 */
#ifndef Mesh_h
#define Mesh_h

#include "entity.h"
#include "scene.h"
#include "core/CPPMetal.hpp"

namespace vox {
EntityPtr newMeshesFromBundlePath(const char *bundlePath,
                                  const char *meshFile,
                                  MTL::Device &device,
                                  Scene* scene,
                                  const MTL::VertexDescriptor &vertexDescriptor);
}

#endif // Mesh_h
