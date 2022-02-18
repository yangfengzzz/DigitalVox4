//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "debug_frame_manager.h"
#include "material/base_material.h"
#include "scene.h"
#include "mesh/mesh_renderer.h"
#include "graphics/mesh.h"

namespace vox {
template<> DebugFrameManager *Singleton<DebugFrameManager>::msSingleton = 0;
//-----------------------------------------------------------------------
DebugFrameManager *DebugFrameManager::getSingletonPtr(void) {
    return msSingleton;
}

DebugFrameManager &DebugFrameManager::getSingleton(void) {
    assert(msSingleton);
    return (*msSingleton);
}

DebugFrameManager::DebugFrameManager(EntityPtr entity):
_entity(entity) {
    _material = std::make_shared<BaseMaterial>(Shader::create("debug_frame", "vertex_debug_frame", "fragment_debug_frame"));
    
    _vertexDescriptor.attributes[0].format(MTL::VertexFormatFloat3);
    _vertexDescriptor.attributes[0].offset(0);
    _vertexDescriptor.attributes[0].bufferIndex(0);
    _vertexDescriptor.attributes[1].format(MTL::VertexFormatUInt);
    _vertexDescriptor.attributes[1].offset(sizeof(uint32_t));
    _vertexDescriptor.attributes[1].bufferIndex(0);
    _vertexDescriptor.layouts[0].stride(sizeof(float) * 4);
    
    _lines.renderer = _entity->addComponent<MeshRenderer>();
    _lines.renderer->setMaterial(_material);
}

void DebugFrameManager::RenderDebugBuffer::clear() {
    vertex.clear();
    indices.clear();
}

void DebugFrameManager::clear() {
    _points.clear();
    _lines.clear();
    _triangles.clear();
}

void DebugFrameManager::addLine(const Vector3F &a, const Vector3F &b, uint32_t color) {
    _lines.vertex.push_back({a, color});
    _lines.indices.push_back(static_cast<uint32_t>(_lines.indices.size()));
    _lines.vertex.push_back({b, color});
    _lines.indices.push_back(static_cast<uint32_t>(_lines.indices.size()));
}

void DebugFrameManager::addLine(const Matrix4x4F &t, const Vector3F &a, const Vector3F &b, unsigned int color) {
    _lines.vertex.push_back({ t * a, color});
    _lines.indices.push_back(static_cast<uint32_t>(_lines.indices.size()));
    _lines.vertex.push_back({ t * b, color});
    _lines.indices.push_back(static_cast<uint32_t>(_lines.indices.size()));
}

void DebugFrameManager::flush() {
    if (!_lines.vertex.empty()) {
        uint32_t vertexByteLength = static_cast<uint32_t>(_lines.vertex.size() * sizeof(RenderDebugVertex));
        
        if (_lines.vertexBuffer == nullptr || _lines.vertexBuffer->length() != vertexByteLength) {
            _lines.vertexBuffer = _entity->scene()->device()->newBufferWithBytes(_lines.vertex.data(), vertexByteLength);
        } else {
            memcpy(_lines.vertexBuffer->contents(), _lines.vertex.data(), vertexByteLength);
        }
        
        if (_lines.indicesBuffer == nullptr || _lines.indicesBuffer->length() != _lines.indices.size() * sizeof(uint32_t)) {
            _lines.indicesBuffer = _entity->scene()->device()->newBufferWithBytes(_lines.indices.data(), _lines.indices.size() * sizeof(uint32_t));
        } else {
            memcpy(_lines.indicesBuffer->contents(), _lines.indices.data(), _lines.indices.size() * sizeof(uint32_t));
        }
        
        auto submesh = Submesh(MTL::PrimitiveTypeLine, MTL::IndexTypeUInt32, _lines.indices.size(),
                               MeshBuffer(*_lines.indicesBuffer, 0, _lines.indices.size() * sizeof(uint32_t)));
        std::vector<MeshBuffer> buffer = {MeshBuffer(*_lines.vertexBuffer, 0, vertexByteLength, 0)};
        _lines.renderer->setMesh(std::make_shared<Mesh>(submesh, buffer, _vertexDescriptor));
    }

    
    clear();
}

}
