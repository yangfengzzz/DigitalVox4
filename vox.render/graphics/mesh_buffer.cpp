//
//  mesh_buffer.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/14.
//

#include "mesh_buffer.h"
#include <set>

using namespace MTL;

namespace vox {
inline MeshBuffer::MeshBuffer(MTL::Buffer buffer,
                              MTL::UInteger offset,
                              MTL::UInteger length,
                              MTL::UInteger argumentIndex)
: m_buffer(buffer), m_offset(offset), m_length(length), m_argumentIndex(argumentIndex) {
    // Member initialization only
}

inline MeshBuffer::MeshBuffer(MTL::UInteger offset,
                              MTL::UInteger length,
                              MTL::UInteger argumentIndex)
: m_buffer(), m_offset(offset), m_length(length), m_argumentIndex(argumentIndex) {
    // Member initialization only
}

MeshBuffer::~MeshBuffer() {
}

size_t alignSize(size_t inSize, size_t alignment) {
    // Asset if align is not a power of 2
    assert(((alignment - 1) & alignment) == 0);
    
    const MTL::UInteger alignmentMask = alignment - 1;
    
    return ((inSize + alignmentMask) & (~alignmentMask));
}

std::vector<MeshBuffer>
MeshBuffer::makeVertexBuffers(MTL::Device &device,
                              const MTL::VertexDescriptor &descritptor,
                              MTL::UInteger vertexCount,
                              MTL::UInteger indexBufferSize) {
    std::set<MTL::UInteger> bufferIndicessUsed;
    
    for (int i = 0; i < MTL::MaxVertexAttributes; i++) {
        bufferIndicessUsed.insert(descritptor.attributes[i].bufferIndex());
    }
    
    std::vector<MeshBuffer> vertexBuffers;
    
    indexBufferSize = alignSize(indexBufferSize, 256);
    
    UInteger bufferLength = indexBufferSize;
    for (auto bufferIndex: bufferIndicessUsed) {
        UInteger offset = bufferLength;
        UInteger sectionLength = alignSize(vertexCount * descritptor.layouts[bufferIndex].stride(), 256);
        
        bufferLength += sectionLength;
        
        MeshBuffer meshBuffer(offset, sectionLength, bufferIndex);
        vertexBuffers.emplace_back(meshBuffer);
    }
    
    Buffer metalBuffer = device.makeBuffer(bufferLength);
    
    for (auto &&vertexBuffer: vertexBuffers) {
        vertexBuffer.m_buffer = metalBuffer;
    }
    
    return vertexBuffers;
}

}
