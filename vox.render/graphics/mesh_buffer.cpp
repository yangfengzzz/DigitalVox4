//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.
#include "mesh_buffer.h"
#include "memory/align.h"
#include <set>

using namespace MTL;

namespace vox {
MeshBuffer::MeshBuffer(MTL::Buffer buffer,
                       MTL::UInteger offset,
                       MTL::UInteger length,
                       MTL::UInteger argumentIndex)
: m_buffer(buffer), m_offset(offset), m_length(length), m_argumentIndex(argumentIndex) {
    // Member initialization only
}

MeshBuffer::MeshBuffer(MTL::UInteger offset,
                       MTL::UInteger length,
                       MTL::UInteger argumentIndex)
: m_buffer(), m_offset(offset), m_length(length), m_argumentIndex(argumentIndex) {
    // Member initialization only
}

MeshBuffer::~MeshBuffer() {
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
