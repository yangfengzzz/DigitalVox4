//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef mesh_hpp
#define mesh_hpp

#include "sub_mesh.h"
#include "bounding_box3.h"
#include "update_flag_manager.h"

namespace vox {
struct Mesh {
public:
    /** The bounding volume of the mesh. */
    BoundingBox3F bounds = BoundingBox3F();
    
    Mesh();
    
    Mesh(const std::vector<Submesh> &submeshes,
         const std::vector<MeshBuffer> &vertexBuffers,
         MTL::VertexDescriptor vertexDescriptor);
    
    Mesh(const Submesh &submesh,
         const std::vector<MeshBuffer> &vertexBuffers,
         MTL::VertexDescriptor vertexDescriptor);
    
    Mesh(const Mesh &rhs) = default;
    
    Mesh(Mesh &&rhs) = default;
    
    Mesh &operator=(Mesh &rhs) = default;
    
    Mesh &operator=(Mesh &&rhs) = default;
    
    virtual ~Mesh();
    
    const std::vector<Submesh> &submeshes() const;
    
    const std::vector<MeshBuffer> &vertexBuffers() const;
    
    const MTL::VertexDescriptor& vertexDescriptor() const;
    
    /**
     * Register update flag, update flag will be true if the vertex element changes.
     * @returns Update flag
     */
    std::unique_ptr<UpdateFlag> registerUpdateFlag();
    
protected:
    std::vector<Submesh> _submeshes;
    
    std::vector<MeshBuffer> _vertexBuffers;
    
    MTL::VertexDescriptor _vertexDescriptor;
    
    UpdateFlagManager _updateFlagManager = UpdateFlagManager();
};

using MeshPtr = std::shared_ptr<Mesh>;

inline const std::vector<Submesh> &Mesh::submeshes() const {
    return _submeshes;
}

inline const std::vector<MeshBuffer> &Mesh::vertexBuffers() const {
    return _vertexBuffers;
}

inline const MTL::VertexDescriptor& Mesh::vertexDescriptor() const {
    return _vertexDescriptor;
}


}

#endif /* mesh_hpp */
