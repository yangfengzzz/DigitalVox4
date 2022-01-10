/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Header for C++ Metal render command encoder dispatch table implementation
*/

#ifndef CPPMetalRenderCommandEncoder_DispatchTable_hpp
#define CPPMetalRenderCommandEncoder_DispatchTable_hpp

#include "CPPMetalBuffer.hpp"
#include "CPPMetalRenderPipeline.hpp"
#include "CPPMetalTexture.hpp"
#include "CPPMetalTypes.hpp"
#include "CPPMetalDepthStencil.hpp"
#include "CPPMetalImplementation.hpp"
#include <objc/message.h>


namespace CPPMetalInternal
{


static const SEL setVertexBytesSel          = sel_registerName("setVertexBytes:length:atIndex:");
static const SEL setVertexBufferSel         = sel_registerName("setVertexBuffer:offset:atIndex:");
static const SEL setVertexBufferOffsetSel   = sel_registerName("setVertexBufferOffset:atIndex:");
static const SEL setVertexTextureSel        = sel_registerName("setVertexTexture:atIndex:");
static const SEL setFragmentBytesSel        = sel_registerName("setFragmentBytes:length:atIndex:");
static const SEL setFragmentBufferSel       = sel_registerName("setFragmentBuffer:offset:atIndex:");
static const SEL setFragmentBufferOffsetSel = sel_registerName("setFragmentBufferOffset:atIndex:");
static const SEL setFragmentTextureSel      = sel_registerName("setFragmentTexture:atIndex:");

typedef void (*setVertexBytesType)          (id, SEL, const void * bytes, MTL::UInteger length, MTL::UInteger index);
typedef void (*setVertexBufferType)         (id, SEL, CPPMetalInternal::Buffer buffer, MTL::UInteger offset, MTL::UInteger index);
typedef void (*setVertexBufferOffsetType)   (id, SEL, MTL::UInteger offset, MTL::UInteger index);
typedef void (*setVertexTextureType)        (id, SEL, CPPMetalInternal::Texture texture, MTL::UInteger index);
typedef void (*setFragmentBytesType)        (id, SEL, const void *bytes, MTL::UInteger length, MTL::UInteger index);
typedef void (*setFragmentBufferType)       (id, SEL, CPPMetalInternal::Buffer buffer, MTL::UInteger offset, MTL::UInteger index);
typedef void (*setFragmentBufferOffsetType) (id, SEL, MTL::UInteger offset, MTL::UInteger index);
typedef void (*setFragmentTextureType)      (id, SEL, CPPMetalInternal::Texture texture, MTL::UInteger index);

struct RenderCommandEncoderDispatchTable
{
    CPP_METAL_DECLARE_FUNCTION_POINTER( setVertexBytes );
    CPP_METAL_DECLARE_FUNCTION_POINTER( setVertexBuffer );
    CPP_METAL_DECLARE_FUNCTION_POINTER( setVertexBufferOffset );
    CPP_METAL_DECLARE_FUNCTION_POINTER( setVertexTexture );
    CPP_METAL_DECLARE_FUNCTION_POINTER( setFragmentBytes );
    CPP_METAL_DECLARE_FUNCTION_POINTER( setFragmentBuffer );
    CPP_METAL_DECLARE_FUNCTION_POINTER( setFragmentBufferOffset );
    CPP_METAL_DECLARE_FUNCTION_POINTER( setFragmentTexture );

    RenderCommandEncoderDispatchTable(CPPMetalInternal::ObjCObj *objCObj);
};

} // namespace CPPMetalInternal

#endif // CPPMetalRenderCommandEncoder_DispatchTable_hpp
