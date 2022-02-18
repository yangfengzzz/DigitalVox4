//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef debug_frame_manager_hpp
#define debug_frame_manager_hpp

#include "entity.h"
#include "singleton.h"
#include "core/cpp_mtl.h"

namespace vox {
class DebugFrameManager : public Singleton<DebugFrameManager> {
public:
    /**
     \brief Default color values used for debug rendering.
     */
    struct DebugColor {
        enum Enum {
            RGB_BLACK = 0xff000000,
            RGB_RED = 0xffff0000,
            RGB_GREEN = 0xff00ff00,
            RGB_BLUE = 0xff0000ff,
            RGB_YELLOW = 0xffffff00,
            RGB_MAGENTA = 0xffff00ff,
            RGB_CYAN = 0xff00ffff,
            RGB_WHITE = 0xffffffff,
            RGB_GREY = 0xff808080,
            RGB_DARKRED = 0x88880000,
            RGB_DARKGREEN = 0x88008800,
            RGB_DARKBLUE = 0x88000088
        };
    };
    
    struct RenderDebugVertex {
        Vector3F pos;
        uint32_t color;
    };
    
    struct RenderDebugBuffer {
        std::vector<RenderDebugVertex> vertex{};
        std::vector<uint32_t> indices{};
        MTL::Buffer* vertexBuffer{nullptr};
        MTL::Buffer* indicesBuffer{nullptr};
        MeshRenderer* renderer{nullptr};
        
        void clear();
    };
    
    static DebugFrameManager &getSingleton(void);
    
    static DebugFrameManager *getSingletonPtr(void);
    
    DebugFrameManager(EntityPtr entity);
    
    void clear();
    
    void flush();
    
public:
    void addLine(const Vector3F &a, const Vector3F &b, uint32_t color);
    
    void addVector(const Vector3F &start, const Vector3F &vec, uint32_t color) {
        addLine(start, start + vec, color);
    }
    
    void addLine(const Matrix4x4F &t, const Vector3F &a, const Vector3F &b, uint32_t color);
    
    void addVector(const Matrix4x4F &t, const Vector3F &start, const Vector3F &vec, uint32_t color) {
        addLine(t, start, start + vec, color);
    }
    
private:
    EntityPtr _entity{nullptr};
    
    MaterialPtr _material{nullptr};
    RenderDebugBuffer _points{};
    RenderDebugBuffer _lines{};
    RenderDebugBuffer _triangles{};
    MTL::VertexDescriptor _vertexDescriptor;
};

}

#endif /* debug_frame_manager_hpp */
