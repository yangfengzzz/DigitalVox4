/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Implementation of C++ Metal render command encoder class wrapper
*/

#include "CPPMetalDevice.hpp"
#include "CPPMetalRenderCommandEncoder.hpp"
#include "CPPMetalInternalMacros.h"
#include "CPPMetalDepthStencil.hpp"
#include "CPPMetalDeviceInternals.h"
#include <Metal/Metal.h>

using namespace MTL;


RenderCommandEncoder::RenderCommandEncoder(const CPPMetalInternal::RenderCommandEncoder objCObj,
                                           Device & device)
: CommandEncoder(objCObj, device)
{
    m_dispatch = m_device->internals().getRenderCommandEncoderTable(objCObj);
}

RenderCommandEncoder::RenderCommandEncoder(const RenderCommandEncoder & rhs)
: CommandEncoder(rhs)
, m_dispatch(rhs.m_dispatch)
{
    // Member initialization only
}

RenderCommandEncoder & RenderCommandEncoder::operator=(const RenderCommandEncoder & rhs)
{
    CommandEncoder::operator=(rhs);

    return *this;
}

RenderCommandEncoder::~RenderCommandEncoder()
{
}


bool RenderCommandEncoder::operator==(const RenderCommandEncoder & rhs) const
{
    return [((id<MTLRenderCommandEncoder>)m_objCObj) isEqual:rhs.m_objCObj];
}

void RenderCommandEncoder::setRenderPipelineState(const RenderPipelineState & pipelineState)
{
    [((id<MTLRenderCommandEncoder>)m_objCObj) setRenderPipelineState:pipelineState.objCObj()];
}

void RenderCommandEncoder::setVertexBuffers(const Buffer *buffers[], const UInteger offsets[], Range range)
{
    __unsafe_unretained id<MTLBuffer> mtlBuffers[range.length];

    for(int i = 0; i < range.length; i++)
    {
        mtlBuffers[i] = buffers[i]->objCObj();
    }
    [((id<MTLRenderCommandEncoder>)m_objCObj) setVertexBuffers:mtlBuffers offsets:offsets withRange:*((NSRange*)&range)];
}

void RenderCommandEncoder::setVertexTextures(const Texture *textures[], Range range)
{
    __unsafe_unretained id<MTLTexture> mtlTextures[range.length];

    for(int i = 0; i < range.length; i++)
    {
        mtlTextures[i] = textures[i]->objCObj();
    }
    [((id<MTLRenderCommandEncoder>)m_objCObj) setVertexTextures:mtlTextures withRange:*((NSRange*)&range)];
}

void RenderCommandEncoder::setViewport(const Viewport & viewport)
{
    MTLViewport *viewportPtr = (MTLViewport*)&viewport;
    [((id<MTLRenderCommandEncoder>)m_objCObj) setViewport:*viewportPtr];
}

void RenderCommandEncoder::setFrontFacingWinding(Winding winding)
{
    [((id<MTLRenderCommandEncoder>)m_objCObj) setFrontFacingWinding:(MTLWinding)winding];
}

void RenderCommandEncoder::setCullMode(CullMode cullMode)
{
    [((id<MTLRenderCommandEncoder>)m_objCObj) setCullMode:(MTLCullMode)cullMode];
}

void RenderCommandEncoder::setDepthClipMode(DepthClipMode depthClipMode)
{
    [((id<MTLRenderCommandEncoder>)m_objCObj) setDepthClipMode:(MTLDepthClipMode) depthClipMode];
}

void RenderCommandEncoder::setDepthBias(float depthBias, float slopeScale, float clamp)
{
    [((id<MTLRenderCommandEncoder>)m_objCObj) setDepthBias:depthBias slopeScale:slopeScale clamp:clamp];
}

void RenderCommandEncoder::setScissorRect(const ScissorRect & scissorRect)
{
    MTLScissorRect *scissorPtr = (MTLScissorRect*)&scissorRect;
    [((id<MTLRenderCommandEncoder>)m_objCObj) setScissorRect:*scissorPtr];
}

void RenderCommandEncoder::setTriangleFillMode(TriangleFillMode fillMode)
{
    [((id<MTLRenderCommandEncoder>)m_objCObj) setTriangleFillMode:(MTLTriangleFillMode) fillMode];
}


void RenderCommandEncoder::setFragmentBuffers(const Buffer *buffers[], const UInteger offsets[], Range range)
{
    __unsafe_unretained id<MTLBuffer> mtlBuffers[range.length];

    for(int i = 0; i < range.length; i++)
    {
        mtlBuffers[i] = buffers[i]->objCObj();
    }
    [((id<MTLRenderCommandEncoder>)m_objCObj) setFragmentBuffers:mtlBuffers offsets:offsets withRange:*((NSRange*)&range)];
}

void RenderCommandEncoder::setFragmentTextures(const Texture *textures[], Range range)
{
    __unsafe_unretained id<MTLTexture> mtlTextures[range.length];

    for(int i = 0; i < range.length; i++)
    {
        mtlTextures[i] = textures[i]->objCObj();
    }
    [((id<MTLRenderCommandEncoder>)m_objCObj) setFragmentTextures:mtlTextures withRange:*((NSRange*)&range)];
}

void RenderCommandEncoder::setBlendColor(float red, float green, float blue, float alpha)
{
    [((id<MTLRenderCommandEncoder>)m_objCObj) setBlendColorRed:red green:green blue:blue alpha:alpha];
}

void RenderCommandEncoder::setDepthStencilState(const DepthStencilState & state)
{
    [((id<MTLRenderCommandEncoder>)m_objCObj) setDepthStencilState:state.objCObj()];
}


void RenderCommandEncoder::setStencilReferenceValue(uint32_t referenceValue)
{
    [((id<MTLRenderCommandEncoder>)m_objCObj) setStencilReferenceValue:referenceValue];
}

void RenderCommandEncoder::drawPrimitives(PrimitiveType primitiveType, UInteger vertexStart, UInteger vertexCount, UInteger instanceCount)
{
    [((id<MTLRenderCommandEncoder>)m_objCObj) drawPrimitives:(MTLPrimitiveType)primitiveType
                                                 vertexStart:vertexStart
                                                 vertexCount:vertexCount
                                               instanceCount:instanceCount];
}

void RenderCommandEncoder::drawIndexedPrimitives(PrimitiveType primitiveType,
                                                 UInteger indexCount,
                                                 IndexType indexType,
                                                 const Buffer & indexBuffer,
                                                 UInteger indexBufferOffset,
                                                 UInteger instanceCount)
{
    [((id<MTLRenderCommandEncoder>)m_objCObj) drawIndexedPrimitives:(MTLPrimitiveType)primitiveType
                                                         indexCount:indexCount
                                                          indexType:(MTLIndexType)indexType
                                                        indexBuffer:indexBuffer.objCObj()
                                                  indexBufferOffset:indexBufferOffset
                       instanceCount:instanceCount];
}

void RenderCommandEncoder::drawPrimitives(PrimitiveType primitiveType,
                                          UInteger vertexStart,
                                          UInteger vertexCount,
                                          UInteger instanceCount,
                                          UInteger baseInstance)
{
    [((id<MTLRenderCommandEncoder>)m_objCObj) drawPrimitives:(MTLPrimitiveType)primitiveType
                                                 vertexStart:vertexStart
                                                 vertexCount:vertexCount
                                               instanceCount:instanceCount
                                                baseInstance:baseInstance];
}

CPP_METAL_VALIDATE_ENUM_ALIAS( IndexTypeUInt16 );
CPP_METAL_VALIDATE_ENUM_ALIAS( IndexTypeUInt32 );

CPP_METAL_VALIDATE_ENUM_ALIAS( PrimitiveTypePoint );
CPP_METAL_VALIDATE_ENUM_ALIAS( PrimitiveTypeLine );
CPP_METAL_VALIDATE_ENUM_ALIAS( PrimitiveTypeLineStrip );
CPP_METAL_VALIDATE_ENUM_ALIAS( PrimitiveTypeTriangle );
CPP_METAL_VALIDATE_ENUM_ALIAS( PrimitiveTypeTriangleStrip );

CPP_METAL_VALIDATE_ENUM_ALIAS( CullModeNone );
CPP_METAL_VALIDATE_ENUM_ALIAS( CullModeFront );
CPP_METAL_VALIDATE_ENUM_ALIAS( CullModeBack );

CPP_METAL_VALIDATE_ENUM_ALIAS( WindingClockwise );
CPP_METAL_VALIDATE_ENUM_ALIAS( WindingCounterClockwise );

CPP_METAL_VALIDATE_ENUM_ALIAS( DepthClipModeClip );
CPP_METAL_VALIDATE_ENUM_ALIAS( DepthClipModeClamp );
CPP_METAL_VALIDATE_ENUM_ALIAS( TriangleFillModeFill );
CPP_METAL_VALIDATE_ENUM_ALIAS( TriangleFillModeLines );

CPP_METAL_VALIDATE_STRUCT_ALIAS( Viewport, originX );
CPP_METAL_VALIDATE_STRUCT_ALIAS( Viewport, originY );
CPP_METAL_VALIDATE_STRUCT_ALIAS( Viewport, width );
CPP_METAL_VALIDATE_STRUCT_ALIAS( Viewport, height );
CPP_METAL_VALIDATE_STRUCT_ALIAS( Viewport, znear );
CPP_METAL_VALIDATE_STRUCT_ALIAS( Viewport, zfar );

CPP_METAL_VALIDATE_STRUCT_ALIAS( ScissorRect, x );
CPP_METAL_VALIDATE_STRUCT_ALIAS( ScissorRect, y );
CPP_METAL_VALIDATE_STRUCT_ALIAS( ScissorRect, width );
CPP_METAL_VALIDATE_STRUCT_ALIAS( ScissorRect, height );

