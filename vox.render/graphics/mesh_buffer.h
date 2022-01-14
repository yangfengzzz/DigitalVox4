//
//  mesh_buffer.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/14.
//

#ifndef mesh_buffer_hpp
#define mesh_buffer_hpp

#include "CPPMetal.hpp"
#include <vector>

namespace vox {
class MeshBuffer {
public:
    // MeshBuffer objects owns MTL::Buffer memory with this constructor
    MeshBuffer(MTL::Buffer buffer,
               MTL::UInteger offset,
               MTL::UInteger length,
               MTL::UInteger argumentIndex = MTL::UIntegerMax);
    
    MeshBuffer(const MeshBuffer &rhs) = default;
    
    MeshBuffer(MeshBuffer &&rhs) = default;
    
    MeshBuffer &operator=(MeshBuffer &rhs) = default;
    
    MeshBuffer &operator=(MeshBuffer &&rhs) = default;
    
    virtual ~MeshBuffer();
    
    const MTL::Buffer &buffer() const;
    
    MTL::UInteger length() const;
    
    MTL::UInteger argumentIndex() const;
    
    MTL::UInteger offset() const;
    
    static std::vector<MeshBuffer>
    makeVertexBuffers(MTL::Device &device,
                      const MTL::VertexDescriptor &descritptor,
                      MTL::UInteger vertexCount,
                      MTL::UInteger indexBufferSize);
    
private:
    
    MTL::Buffer m_buffer;
    MTL::UInteger m_length;
    MTL::UInteger m_offset;
    MTL::UInteger m_argumentIndex;
    
    MeshBuffer(MTL::UInteger offset,
               MTL::UInteger length,
               MTL::UInteger argumentIndex = MTL::UIntegerMax);
};

inline const MTL::Buffer &MeshBuffer::buffer() const {
    return m_buffer;
}

inline MTL::UInteger MeshBuffer::offset() const {
    return m_offset;
}

inline MTL::UInteger MeshBuffer::length() const {
    return m_length;
}

inline MTL::UInteger MeshBuffer::argumentIndex() const {
    return m_argumentIndex;
}

}

#endif /* mesh_buffer_hpp */
