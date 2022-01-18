//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "primitive_mesh.h"
#include "rendering/math_utilities.h"

// Include header shared between C code here, which executes Metal API commands, and .metal files
#include "shader_types.h"

using namespace MTL;

namespace vox {
namespace {
void packVertexData(void *output, VertexFormat format, vector_float4 value) {
    switch (format) {
        case VertexFormatUChar4Normalized:
            ((uint8_t *) output)[3] = 0xFF * value.w;
        case VertexFormatUChar3Normalized:
            ((uint8_t *) output)[2] = 0xFF * value.z;
        case VertexFormatUChar2Normalized:
            ((uint8_t *) output)[1] = 0xFF * value.y;
            ((uint8_t *) output)[0] = 0xFF * value.x;
            break;
        case VertexFormatChar4Normalized:
            ((int8_t *) output)[3] = 0x7F * (2.0 * value.w - 1.0);
        case VertexFormatChar3Normalized:
            ((int8_t *) output)[2] = 0x7F * (2.0 * value.z - 1.0);
        case VertexFormatChar2Normalized:
            ((int8_t *) output)[1] = 0x7F * (2.0 * value.y - 1.0);
            ((int8_t *) output)[0] = 0x7F * (2.0 * value.x - 1.0);
            break;
        case VertexFormatUShort4Normalized:
            ((uint16_t *) output)[3] = 0xFFFF * (2.0 * value.w - 1.0);
        case VertexFormatUShort3Normalized:
            ((uint16_t *) output)[2] = 0xFFFF * (2.0 * value.z - 1.0);
        case VertexFormatUShort2Normalized:
            ((uint16_t *) output)[1] = 0xFFFF * (2.0 * value.y - 1.0);
            ((uint16_t *) output)[0] = 0xFFFF * (2.0 * value.x - 1.0);
            break;
        case VertexFormatShort4Normalized:
            ((int16_t *) output)[3] = 0x7FFF * (2.0 * value.w - 1.0);
        case VertexFormatShort3Normalized:
            ((int16_t *) output)[2] = 0x7FFF * (2.0 * value.z - 1.0);
        case VertexFormatShort2Normalized:
            ((int16_t *) output)[1] = 0x7FFF * (2.0 * value.y - 1.0);
            ((int16_t *) output)[0] = 0x7FFF * (2.0 * value.x - 1.0);
            break;
        case VertexFormatHalf4:
            ((__fp16 *) output)[3] = value.w;
        case VertexFormatHalf3:
            ((__fp16 *) output)[2] = value.z;
        case VertexFormatHalf2:
            ((__fp16 *) output)[1] = value.y;
            ((__fp16 *) output)[0] = value.x;
            break;
        case VertexFormatFloat4:
            ((float *) output)[3] = value.w;
        case VertexFormatFloat3:
            ((float *) output)[2] = value.z;
        case VertexFormatFloat2:
            ((float *) output)[1] = value.y;
        case VertexFormatFloat:
            ((float *) output)[0] = value.x;
            break;
        default:
            break;
    }
}

} // namespace

MeshPtr PrimitiveMesh::makeSphereMesh(MTL::Device &device,
                                   const MTL::VertexDescriptor &vertexDescriptor,
                                   int radialSegments, int verticalSegments, float radius) {
    const UInteger vertexCount = 2 + (radialSegments) * (verticalSegments - 1);
    const UInteger indexCount = 6 * radialSegments * (verticalSegments - 1);;
    
    const UInteger indexBufferSize = indexCount * sizeof(ushort);
    
    assert(vertexCount < UINT16_MAX);
    
    std::vector<MeshBuffer> vertexBuffers;
    
    vertexBuffers = MeshBuffer::makeVertexBuffers(device,
                                                  vertexDescriptor,
                                                  vertexCount,
                                                  indexBufferSize);
    
    Buffer metalBuffer = vertexBuffers[0].buffer();
    
    // Create index buffer from the Metal buffer shared with the vertices and resrve space a the
    // beginning for indices
    MeshBuffer indexBuffer(metalBuffer, 0, indexBufferSize);
    
    uint8_t *bufferContents = (uint8_t *) metalBuffer.contents();
    
    // Fill IndexBuffer
    {
        ushort *indicies = (ushort *) bufferContents;
        
        UInteger currentIndex = 0;
        
        // Indices for top of sphere
        for (ushort phi = 0; phi < radialSegments; phi++) {
            if (phi < radialSegments - 1) {
                indicies[currentIndex++] = 0;
                indicies[currentIndex++] = 2 + phi;
                indicies[currentIndex++] = 1 + phi;
            } else {
                indicies[currentIndex++] = 0;
                indicies[currentIndex++] = 1;
                indicies[currentIndex++] = 1 + phi;
            }
        }
        
        // Indices middle of sphere
        for (ushort theta = 0; theta < verticalSegments - 2; theta++) {
            ushort topRight;
            ushort topLeft;
            ushort bottomRight;
            ushort bottomLeft;
            
            for (ushort phi = 0; phi < radialSegments; phi++) {
                if (phi < radialSegments - 1) {
                    topRight = 1 + theta * (radialSegments) + phi;
                    topLeft = 1 + theta * (radialSegments) + (phi + 1);
                    bottomRight = 1 + (theta + 1) * (radialSegments) + phi;
                    bottomLeft = 1 + (theta + 1) * (radialSegments) + (phi + 1);
                } else {
                    topRight = 1 + theta * (radialSegments) + phi;
                    topLeft = 1 + theta * (radialSegments);
                    bottomRight = 1 + (theta + 1) * (radialSegments) + phi;
                    bottomLeft = 1 + (theta + 1) * (radialSegments);
                }
                
                indicies[currentIndex++] = topRight;
                indicies[currentIndex++] = bottomLeft;
                indicies[currentIndex++] = bottomRight;
                
                indicies[currentIndex++] = topRight;
                indicies[currentIndex++] = topLeft;
                indicies[currentIndex++] = bottomLeft;
            }
        }
        
        // Indicies for bottom of sphere
        ushort lastIndex = radialSegments * (verticalSegments - 1) + 1;
        for (ushort phi = 0; phi < radialSegments; phi++) {
            if (phi < radialSegments - 1) {
                indicies[currentIndex++] = lastIndex;
                indicies[currentIndex++] = lastIndex - radialSegments + phi;
                indicies[currentIndex++] = lastIndex - radialSegments + phi + 1;
            } else {
                indicies[currentIndex++] = lastIndex;
                indicies[currentIndex++] = lastIndex - radialSegments + phi;
                indicies[currentIndex++] = lastIndex - radialSegments;
            }
        }
    }
    
    // Fill positions and normals
    {
        VertexFormat postitionFormat = vertexDescriptor.attributes[VertexAttributePosition].format();
        UInteger positionBufferIndex = vertexDescriptor.attributes[VertexAttributePosition].bufferIndex();
        UInteger positionVertexOffset = vertexDescriptor.attributes[VertexAttributePosition].offset();
        UInteger positionBufferOffset = vertexBuffers[positionBufferIndex].offset();
        UInteger positionStride = vertexDescriptor.layouts[positionBufferIndex].stride();
        
        VertexFormat normalFormat = vertexDescriptor.attributes[VertexAttributeNormal].format();
        UInteger normalBufferIndex = vertexDescriptor.attributes[VertexAttributeNormal].bufferIndex();
        UInteger normalVertexOffset = vertexDescriptor.attributes[VertexAttributeNormal].offset();
        UInteger normalBufferOffset = vertexBuffers[normalBufferIndex].offset();
        UInteger normalStride = vertexDescriptor.layouts[normalBufferIndex].stride();
        
        const double radialDelta = 2 * (M_PI / radialSegments);
        const double verticalDelta = (M_PI / verticalSegments);
        
        uint8_t *positionData = bufferContents + positionBufferOffset + positionVertexOffset;
        uint8_t *normalData = bufferContents + normalBufferOffset + normalVertexOffset;
        
        vector_float4 vertexPosition = {0, radius, 0, 1};
        vector_float4 vertexNormal = {0, 1, 0, 1};;
        
        packVertexData(positionData, postitionFormat, vertexPosition);
        packVertexData(normalData, normalFormat, vertexNormal);
        
        positionData += positionStride;
        normalData += normalStride;
        
        for (ushort verticalSegment = 1; verticalSegment < verticalSegments; verticalSegment++) {
            const double verticalPosition = verticalSegment * verticalDelta;
            
            float y = cos(verticalPosition);
            
            for (ushort radialSegment = 0; radialSegment < radialSegments; radialSegment++) {
                const double radialPositon = radialSegment * radialDelta;
                
                vector_float4 unscaledPositon;
                
                unscaledPositon.x = sin(verticalPosition) * cos(radialPositon);
                unscaledPositon.y = y;
                unscaledPositon.z = sin(verticalPosition) * sin(radialPositon);
                unscaledPositon.w = 1.0;
                
                vertexPosition = radius * unscaledPositon;
                vertexNormal = unscaledPositon;
                
                packVertexData(positionData, postitionFormat, vertexPosition);
                packVertexData(normalData, normalFormat, vertexNormal);
                
                positionData += positionStride;
                normalData += normalStride;
                
            }
        }
        
        vertexPosition = {0, -radius, 0, 1};
        vertexNormal = {0, -1, 0, 1};;
        
        packVertexData(positionData, postitionFormat, vertexPosition);
        packVertexData(normalData, normalFormat, vertexNormal);
        
        positionData += positionStride;
        normalData += normalStride;
        
    }
    
    Submesh submesh(PrimitiveTypeTriangle,
                    IndexTypeUInt16,
                    indexCount,
                    indexBuffer);
    
    return std::make_shared<Mesh>(submesh, vertexBuffers, vertexDescriptor);
}


MeshPtr PrimitiveMesh::makeIcosahedronMesn(MTL::Device &device,
                                        const MTL::VertexDescriptor &vertexDescriptor,
                                        float radius) {
    const float Z = radius;
    const float X = (Z / (1.0 + sqrtf(5.0))) * 2;
    const vector_float4 positions[] =
    {
        {-X, 0.0, Z},
        {X, 0.0, Z},
        {-X, 0.0, -Z},
        {X, 0.0, -Z},
        {0.0, Z, X},
        {0.0, Z, -X},
        {0.0, -Z, X},
        {0.0, -Z, -X},
        {Z, X, 0.0},
        {-Z, X, 0.0},
        {Z, -X, 0.0},
        {-Z, -X, 0.0}
    };
    
    const uint16_t vertexCount = sizeof(positions) / sizeof(vector_float3);
    
    const uint16_t indices[][3] =
    {
        {0, 1, 4},
        {0, 4, 9},
        {9, 4, 5},
        {4, 8, 5},
        {4, 1, 8},
        {8, 1, 10},
        {8, 10, 3},
        {5, 8, 3},
        {5, 3, 2},
        {2, 3, 7},
        {7, 3, 10},
        {7, 10, 6},
        {7, 6, 11},
        {11, 6, 0},
        {0, 6, 1},
        {6, 10, 1},
        {9, 11, 0},
        {9, 2, 11},
        {9, 5, 2},
        {7, 11, 2}
    };
    
    UInteger indexCount = sizeof(indices) / sizeof(uint16_t);
    UInteger indexBufferSize = sizeof(indices);
    
    std::vector<MeshBuffer> vertexBuffers = MeshBuffer::makeVertexBuffers(device, vertexDescriptor,
                                                                          vertexCount, indexBufferSize);
    
    MTL::Buffer buffer = vertexBuffers[0].buffer();
    
    MeshBuffer indexBuffer(buffer, 0, indexBufferSize);
    
    uint8_t *bufferContents = (uint8_t *) buffer.contents();
    
    memcpy(bufferContents, indices, indexBufferSize);
    
    {
        VertexFormat postitionFormat = vertexDescriptor.attributes[VertexAttributePosition].format();
        UInteger positionBufferIndex = vertexDescriptor.attributes[VertexAttributePosition].bufferIndex();
        UInteger positionVertexOffset = vertexDescriptor.attributes[VertexAttributePosition].offset();
        UInteger positionBufferOffset = vertexBuffers[positionBufferIndex].offset();
        UInteger positionStride = vertexDescriptor.layouts[positionBufferIndex].stride();
        
        
        uint8_t *positionData = bufferContents + positionBufferOffset + positionVertexOffset;
        
        for (uint16_t vertexIndex = 0; vertexIndex < vertexCount; vertexIndex++) {
            packVertexData(positionData, postitionFormat, positions[vertexIndex]);
            positionData += positionStride;
        }
    }
    
    Submesh submesh(PrimitiveTypeTriangle,
                    IndexTypeUInt16,
                    indexCount,
                    indexBuffer);
    
    return std::make_shared<Mesh>(submesh, vertexBuffers, vertexDescriptor);
}

}
