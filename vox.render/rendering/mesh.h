/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Header for mesh and submesh objects used for managing models
*/
#ifndef Mesh_h
#define Mesh_h
#include "CPPMetal.hpp"
#include <unordered_map>

#include "shader_types.h"
#include <vector>

struct MeshVertex
{
    vector_float3 position;
    vector_float2 texcoord;
    vector_float3 normal;
    vector_float3 tangent;
    vector_float3 bitangent;
};

class MeshBuffer
{
public:

    // MeshBuffer objects owns MTL::Buffer memory with this constructor
    MeshBuffer(MTL::Buffer buffer,
               MTL::UInteger offset,
               MTL::UInteger length,
               MTL::UInteger argumentIndex = MTL::UIntegerMax);

    MeshBuffer(const MeshBuffer & rhs) = default;

    MeshBuffer(MeshBuffer && rhs) = default;

    MeshBuffer & operator=(MeshBuffer & rhs) = default;

    MeshBuffer & operator=(MeshBuffer && rhs) = default;

    virtual ~MeshBuffer();

    const MTL::Buffer & buffer() const;
    MTL::UInteger length() const;
    MTL::UInteger argumentIndex() const;
    MTL::UInteger offset() const;

    static std::vector<MeshBuffer>
    makeVertexBuffers(MTL::Device & device,
                      const MTL::VertexDescriptor & descritptor,
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


// App specific submesh class containing data to draw a submesh
struct Submesh
{
public:

    Submesh(MTL::PrimitiveType primitiveType,
            MTL::IndexType indexType,
            MTL::UInteger indexCount,
            const MeshBuffer indexBuffer,
            const std::vector<MTL::Texture> & textures);

    Submesh(MTL::PrimitiveType primitiveType,
            MTL::IndexType indexType,
            MTL::UInteger indexCount,
            MeshBuffer indexBuffer);

    Submesh(const Submesh & rhs) = default;

    Submesh(Submesh && rhs) = default;

    Submesh & operator=(Submesh & rhs) = default;

    Submesh & operator=(Submesh && rhs) = default;

    virtual ~Submesh();

    MTL::PrimitiveType primitiveType() const;
    MTL::IndexType indexType() const;
    MTL::UInteger indexCount() const;
    const MeshBuffer & indexBuffer() const;
    const std::vector<MTL::Texture> & textures() const;

private:

    MTL::PrimitiveType m_primitiveType;

    MTL::IndexType m_indexType;

    MTL::UInteger m_indexCount;

    MeshBuffer m_indexBuffer;

    std::vector<MTL::Texture> m_textures;
};

struct Mesh
{
public:

    Mesh();

    Mesh(const std::vector<Submesh> & submeshes,
         const std::vector<MeshBuffer> & vertexBuffers);

    Mesh(const Submesh & submesh,
         const std::vector<MeshBuffer> & vertexBuffers);

    Mesh(const Mesh & rhs) = default;

    Mesh(Mesh && rhs) = default;

    Mesh & operator=(Mesh & rhs) = default;

    Mesh & operator=(Mesh && rhs) = default;

    virtual ~Mesh();

    const std::vector<Submesh> & submeshes() const;

    const std::vector<MeshBuffer> & vertexBuffers() const;

private:

    std::vector<Submesh> m_submeshes;

    std::vector<MeshBuffer> m_vertexBuffers;
};

std::vector<Mesh> *newMeshesFromBundlePath(const char* bundlePath,
                                           MTL::Device & device,
                                           const MTL::VertexDescriptor & vertexDescriptor,
                                           CFErrorRef *error);


Mesh makeSphereMesh(MTL::Device & device,
                    const MTL::VertexDescriptor & vertexDescriptor,
                    int radialSegments, int verticalSegments, float radius);

Mesh makeIcosahedronMesn(MTL::Device & device,
                         const MTL::VertexDescriptor & vertexDescriptor,
                         float radius);

#pragma mark - MeshBuffer inline implementations

inline const MTL::Buffer & MeshBuffer::buffer() const 
{
    return m_buffer;
}

inline MTL::UInteger MeshBuffer::offset() const
{
    return m_offset;
}

inline MTL::UInteger MeshBuffer::length() const
{
    return m_length;
}

inline MTL::UInteger MeshBuffer::argumentIndex() const
{
    return m_argumentIndex;
}

#pragma mark - Submesh inline implementations

inline MTL::PrimitiveType Submesh::primitiveType() const
{
    return m_primitiveType;
}

inline MTL::IndexType Submesh::indexType() const
{
    return m_indexType;
}

inline MTL::UInteger Submesh::indexCount() const
{
    return m_indexCount;
}

inline const MeshBuffer & Submesh::indexBuffer() const
{
    return m_indexBuffer;
}

#pragma mark - Mesh inline implementations

inline const std::vector<MTL::Texture> & Submesh::textures() const
{
    return m_textures;
}

inline const std::vector<Submesh> & Mesh::submeshes() const
{
    return m_submeshes;
}

inline const std::vector<MeshBuffer> & Mesh::vertexBuffers() const
{
    return m_vertexBuffers;
}



#endif // Mesh_h
