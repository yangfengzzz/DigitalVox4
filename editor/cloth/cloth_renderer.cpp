//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "cloth_renderer.h"
#include "shader_common.h"
#include "graphics/mesh.h"
#include "entity.h"
#include "scene.h"
#include <foundation/PxStrideIterator.h>

namespace vox {
namespace cloth {
namespace {
template<typename T>
void gatherIndices(std::vector<uint16_t> &indices,
                   const nv::cloth::BoundedData &triangles, const nv::cloth::BoundedData &quads) {
    physx::PxStrideIterator<const T> tIt, qIt;
    
    indices.reserve(triangles.count * 3 + quads.count * 6);
    
    tIt = physx::PxMakeIterator(reinterpret_cast<const T *>(triangles.data), triangles.stride);
    for (physx::PxU32 i = 0; i < triangles.count; ++i, ++tIt) {
        indices.push_back(static_cast<uint16_t>(tIt.ptr()[0]));
        indices.push_back(static_cast<uint16_t>(tIt.ptr()[1]));
        indices.push_back(static_cast<uint16_t>(tIt.ptr()[2]));
    }
    
    //Only do quads in case there wasn't triangle data provided
    // otherwise we risk to render triangles double
    if (indices.size() == 0) {
        qIt = physx::PxMakeIterator(reinterpret_cast<const T *>(quads.data), quads.stride);
        for (physx::PxU32 i = 0; i < quads.count; ++i, ++qIt) {
            indices.push_back(static_cast<uint16_t>(qIt.ptr()[0]));
            indices.push_back(static_cast<uint16_t>(qIt.ptr()[1]));
            indices.push_back(static_cast<uint16_t>(qIt.ptr()[2]));
            indices.push_back(static_cast<uint16_t>(qIt.ptr()[0]));
            indices.push_back(static_cast<uint16_t>(qIt.ptr()[2]));
            indices.push_back(static_cast<uint16_t>(qIt.ptr()[3]));
        }
    }
}
} // namespace

ClothRenderer::ClothRenderer(Entity *entity) :
Renderer(entity) {
    
}

void ClothRenderer::_render(std::vector<RenderElement> &opaqueQueue,
                            std::vector<RenderElement> &alphaTestQueue,
                            std::vector<RenderElement> &transparentQueue) {
    shaderData.disableMacro(HAS_UV);
    shaderData.disableMacro(HAS_NORMAL);
    shaderData.disableMacro(HAS_TANGENT);
    shaderData.disableMacro(HAS_VERTEXCOLOR);
    
    shaderData.enableMacro(HAS_NORMAL);

    auto &subMeshes = _mesh->submeshes();
    for (size_t i = 0; i < subMeshes.size(); i++) {
        MaterialPtr material;
        if (i < _materials.size()) {
            material = _materials[i];
        } else {
            material = nullptr;
        }
        if (material != nullptr) {
            RenderElement element(this, _mesh, &subMeshes[i], material);
            pushPrimitive(element, opaqueQueue, alphaTestQueue, transparentQueue);
        }
    }
}

void ClothRenderer::setClothMeshDesc(const nv::cloth::ClothMeshDesc &desc) {
    uint32_t numVertices = desc.points.count;
    _vertices.resize(numVertices);
    
    physx::PxStrideIterator<const physx::PxVec3> pIt(reinterpret_cast<const physx::PxVec3 *>(desc.points.data), desc.points.stride);
    for (physx::PxU32 i = 0; i < numVertices; ++i) {
        _vertices[i].position = *pIt++;
        _vertices[i].normal = physx::PxVec3(0.f);
    }
    
    // build triangle indices
    if (desc.flags & nv::cloth::MeshFlag::e16_BIT_INDICES)
        gatherIndices<physx::PxU16>(_indices, desc.triangles, desc.quads);
    else
        gatherIndices<physx::PxU32>(_indices, desc.triangles, desc.quads);
    
    for (physx::PxU32 i = 0; i < _indices.size(); i += 3) {
        auto p0 = _vertices[_indices[i]].position;
        auto p1 = _vertices[_indices[i + 1]].position;
        auto p2 = _vertices[_indices[i + 2]].position;
        
        auto normal = ((p2 - p0).cross(p1 - p0)).getNormalized();
        
        _vertices[_indices[i]].normal += normal;
        _vertices[_indices[i + 1]].normal += normal;
        _vertices[_indices[i + 2]].normal += normal;
    }
    
    for (physx::PxU32 i = 0; i < numVertices; ++i)
        _vertices[i].normal.normalize();
    
    _vertexDescriptor.attributes[Position].format(MTL::VertexFormatFloat3);
    _vertexDescriptor.attributes[Position].offset(0);
    _vertexDescriptor.attributes[Position].bufferIndex(0);
    _vertexDescriptor.attributes[Normal].format(MTL::VertexFormatFloat3);
    _vertexDescriptor.attributes[Normal].offset(sizeof(float) * 3);
    _vertexDescriptor.attributes[Normal].bufferIndex(0);
    _vertexDescriptor.layouts[0].stride(sizeof(float) * 6);
    
    _initialize(_vertices.data(), (uint32_t) _vertices.size(), sizeof(Vertex),
                _indices.data(), (uint32_t) _indices.size() / 3);
}

void ClothRenderer::_initialize(const void *vertices, uint32_t numVertices, uint32_t vertexSize,
                                const uint16_t *faces, uint32_t numFaces) {
    _numVertices = numVertices;
    _vertexSize = vertexSize;
    _numFaces = numFaces;
    // VB
    {
        _vertexBuffers = _entity->scene()->device()->makeBuffer(vertices, vertexSize * numVertices);
    }
    
    // IB
    if (faces != nullptr) {
        _indexBuffers = _entity->scene()->device()->makeBuffer(faces, sizeof(uint16_t) * numFaces * 3);
        
        auto submesh = Submesh(MTL::PrimitiveTypeTriangle, MTL::IndexTypeUInt16, numFaces * 3,
                               MeshBuffer(_indexBuffers, 0, numFaces * 3 * sizeof(uint16_t)));
        std::vector<MeshBuffer> buffer = {MeshBuffer(_vertexBuffers, 0, sizeof(Vertex) * _vertices.size(), 0)};
        _mesh = std::make_shared<Mesh>(submesh, buffer, _vertexDescriptor);
    }
}

void ClothRenderer::update(const physx::PxVec3 *positions, uint32_t numVertices) {
    physx::PxStrideIterator<const physx::PxVec3> pIt(positions, sizeof(physx::PxVec3));
    Vertex *vertices = _vertices.data();
    const uint16_t *indices = _indices.data();
    for (physx::PxU32 i = 0; i < numVertices; ++i) {
        vertices[i].position = *pIt++;
        vertices[i].normal = physx::PxVec3(0.f);
    }
    
    const physx::PxU32 numIndices = (physx::PxU32) _indices.size();
    for (physx::PxU32 i = 0; i < numIndices; i += 3) {
        const auto p0 = vertices[indices[i]].position;
        const auto p1 = vertices[indices[i + 1]].position;
        const auto p2 = vertices[indices[i + 2]].position;
        
        const auto normal = ((p2 - p0).cross(p1 - p0)).getNormalized();
        
        vertices[indices[i]].normal += normal;
        vertices[indices[i + 1]].normal += normal;
        vertices[indices[i + 2]].normal += normal;
    }
    
    for (physx::PxU32 i = 0; i < numVertices; ++i)
        vertices[i].normal.normalize();
    
    memcpy(_vertexBuffers.contents(), _vertices.data(), sizeof(Vertex) * _vertices.size());
}


}
}
