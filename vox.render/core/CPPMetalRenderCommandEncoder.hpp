/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Header for C++ Metal render command encoder class wrapper
*/

#ifndef CPPMetalRenderCommandEncoder_hpp
#define CPPMetalRenderCommandEncoder_hpp

#include "CPPMetalRenderCommandEncoder_DispatchTable.hpp"
#include "CPPMetalCommandEncoder.hpp"


namespace MTL
{

typedef enum IndexType {
    IndexTypeUInt16 = 0,
    IndexTypeUInt32 = 1,
} IndexType API_AVAILABLE(macos(10.11), ios(8.0));

typedef enum PrimitiveType {
    PrimitiveTypePoint = 0,
    PrimitiveTypeLine = 1,
    PrimitiveTypeLineStrip = 2,
    PrimitiveTypeTriangle = 3,
    PrimitiveTypeTriangleStrip = 4,
} PrimitiveType API_AVAILABLE(macos(10.11), ios(8.0));

typedef struct ScissorRect {
    UInteger x, y, width, height;
} ScissorRect;

typedef struct Viewport {
    double originX, originY, width, height, znear, zfar;
} Viewport;

typedef enum CullMode {
    CullModeNone = 0,
    CullModeFront = 1,
    CullModeBack = 2,
} CullMode API_AVAILABLE(macos(10.11), ios(8.0));

typedef enum Winding {
    WindingClockwise = 0,
    WindingCounterClockwise = 1,
} Winding API_AVAILABLE(macos(10.11), ios(8.0));

typedef enum DepthClipMode {
    DepthClipModeClip = 0,
    DepthClipModeClamp = 1,
} DepthClipMode API_AVAILABLE(macos(10.11), ios(9.0));

typedef enum TriangleFillMode {
    TriangleFillModeFill = 0,
    TriangleFillModeLines = 1,
} TriangleFillMode API_AVAILABLE(macos(10.11), ios(8.0));

class RenderCommandEncoder : public CommandEncoder
{
public:

    RenderCommandEncoder() = delete;

    RenderCommandEncoder(const RenderCommandEncoder & rhs);

    RenderCommandEncoder(RenderCommandEncoder && rhs);

    RenderCommandEncoder & operator=(const RenderCommandEncoder & rhs);

    RenderCommandEncoder & operator=(RenderCommandEncoder && rhs);

    CPP_METAL_VIRTUAL ~RenderCommandEncoder();

    bool operator==(const RenderCommandEncoder & rhs) const;

    // Render State

    void setRenderPipelineState(const RenderPipelineState & pipelineState);

    // Vertex Resources

    void setVertexBytes(const void *bytes, UInteger length, UInteger index);
    void setVertexBuffer(const Buffer & buffer, UInteger offset, UInteger index);
    void setVertexBufferOffset(UInteger offset, UInteger index);
    void setVertexBuffers(const Buffer *buffers[], const UInteger offsets[], Range range);
    void setVertexTexture(const Texture & texture, UInteger index);
    void setVertexTextures(const Texture *textures[], Range range);

    // Render State

    void setViewport(const Viewport & viewport);

    void setFrontFacingWinding(Winding winding);

    void setCullMode(CullMode cullMode);

    void setDepthClipMode(DepthClipMode depthClipMode) API_AVAILABLE(macos(10.11), ios(11.0));

    void setDepthBias(float depthBias, float slopeScale, float clamp);

    void setScissorRect(const ScissorRect & scissorRect);

    void setTriangleFillMode(TriangleFillMode fillMode);

    // Fragment Resources

    void setFragmentBytes(const void *bytes, UInteger length, UInteger index);
    void setFragmentBuffer(const Buffer & buffer, UInteger offset, UInteger index);
    void setFragmentBufferOffset(UInteger offset, UInteger index);
    void setFragmentBuffers(const Buffer *buffers[], const UInteger offsets[], Range range);
    void setFragmentTexture(const Texture & texture, UInteger index);
    void setFragmentTextures(const Texture *textures[], Range range);

    // Render State

    void setBlendColor(float red, float green, float blue, float alpha);

    void setDepthStencilState(const DepthStencilState & state);

    void setStencilReferenceValue(uint32_t referenceValue);

    // Draw Calls

    void drawPrimitives(PrimitiveType primitiveType, UInteger vertexStart, UInteger vertexCount, UInteger instanceCount = 1);

    void drawIndexedPrimitives(PrimitiveType primitiveType,
                               UInteger indexCount,
                               IndexType indexType,
                               const Buffer & indexBuffer,
                               UInteger indexBufferOffset,
                               UInteger instanceCount = 1);

    void drawPrimitives(PrimitiveType primitiveType,
                         UInteger vertexStart,
                         UInteger vertexCount,
                         UInteger instanceCount,
                         UInteger baseInstance) API_AVAILABLE(macos(10.11), ios(9.0));


    void drawIndexedPrimitives(PrimitiveType primitiveType,
                               UInteger indexCount,
                               IndexType indexType,
                               const Buffer & indexBuffer,
                               UInteger indexBufferOffset,
                               UInteger instanceCount,
                               UInteger baseVertex,
                               UInteger baseInstance) API_AVAILABLE(macos(10.11), ios(9.0));
private:

    CPPMetalInternal::RenderCommandEncoderDispatchTable *m_dispatch;

public: // Public methods for CPPMetal internal implementation

    RenderCommandEncoder(const CPPMetalInternal::RenderCommandEncoder objCObj, Device & device);

};


//===============================================================
#pragma mark - RenderCommandEncoder inline method implementations

inline RenderCommandEncoder::RenderCommandEncoder(RenderCommandEncoder && rhs)
: CommandEncoder(rhs)
, m_dispatch(rhs.m_dispatch)
{
    // Member initialization only
}

inline RenderCommandEncoder & RenderCommandEncoder::operator=(RenderCommandEncoder && rhs)
{
    CommandEncoder::operator=(rhs);
    m_dispatch = rhs.m_dispatch;

    return *this;
}


//===============================================================
#pragma mark - RenderCommandEncoder inline method implementations

inline void RenderCommandEncoder::setVertexBytes(const void *bytes, UInteger length, UInteger index)
{
    m_dispatch->setVertexBytes(m_objCObj, CPPMetalInternal::setVertexBytesSel, bytes, length, index);
}

inline void RenderCommandEncoder::setVertexBuffer(const Buffer & buffer, UInteger offset, UInteger index)
{
    m_dispatch->setVertexBuffer(m_objCObj, CPPMetalInternal::setVertexBufferSel, buffer.objCObj(), offset, index);
}

inline void RenderCommandEncoder::setVertexBufferOffset(UInteger offset, UInteger index)
{
    m_dispatch->setVertexBufferOffset(m_objCObj, CPPMetalInternal::setVertexBufferOffsetSel, offset, index);
}

inline void RenderCommandEncoder::setVertexTexture(const Texture & texture, UInteger index)
{
    m_dispatch->setVertexTexture(m_objCObj, CPPMetalInternal::setVertexTextureSel, texture.objCObj(), index);
}

inline void RenderCommandEncoder::setFragmentBytes(const void *bytes, UInteger length, UInteger index)
{
    m_dispatch->setFragmentBytes(m_objCObj, CPPMetalInternal::setFragmentBytesSel, bytes, length, index);
}

inline void RenderCommandEncoder::setFragmentBuffer(const Buffer & buffer, UInteger offset, UInteger index)
{
    m_dispatch->setFragmentBuffer(m_objCObj, CPPMetalInternal::setFragmentBufferSel, buffer.objCObj(), offset, index);
}

inline void RenderCommandEncoder::setFragmentBufferOffset(UInteger offset, UInteger index)
{
    m_dispatch->setFragmentBufferOffset(m_objCObj, CPPMetalInternal::setFragmentBufferOffsetSel, offset, index);
}

inline void RenderCommandEncoder::setFragmentTexture(const Texture & texture, UInteger index)
{
    m_dispatch->setFragmentTexture(m_objCObj, CPPMetalInternal::setFragmentTextureSel, texture.objCObj(), index);
}


} // namespace MTL

#endif // CPPMetalRenderCommandEncoder_hpp
