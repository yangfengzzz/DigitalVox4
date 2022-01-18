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
void newMeshesFromBundlePath(const char *bundlePath,
                             const char *meshFile,
                             MTL::Device &device,
                             EntityPtr entity,
                             const MTL::VertexDescriptor &vertexDescriptor);
}

#endif // Mesh_h
