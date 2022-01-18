//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef cpp_mtl_RenderCommandEncoder_DispatchTable_hpp
#define cpp_mtl_RenderCommandEncoder_DispatchTable_hpp

#include "cpp_mtl_buffer.h"
#include "cpp_mtl_render_pipeline.h"
#include "cpp_mtl_texture.h"
#include "cpp_mtl_types.h"
#include "cpp_mtl_depth_stencil.h"
#include "cpp_mtl_implementation.h"
#include <objc/message.h>


namespace cpp_mtl_Internal {


static const SEL setVertexBytesSel = sel_registerName("setVertexBytes:length:atIndex:");
static const SEL setVertexBufferSel = sel_registerName("setVertexBuffer:offset:atIndex:");
static const SEL setVertexBufferOffsetSel = sel_registerName("setVertexBufferOffset:atIndex:");
static const SEL setVertexTextureSel = sel_registerName("setVertexTexture:atIndex:");
static const SEL setFragmentBytesSel = sel_registerName("setFragmentBytes:length:atIndex:");
static const SEL setFragmentBufferSel = sel_registerName("setFragmentBuffer:offset:atIndex:");
static const SEL setFragmentBufferOffsetSel = sel_registerName("setFragmentBufferOffset:atIndex:");
static const SEL setFragmentTextureSel = sel_registerName("setFragmentTexture:atIndex:");

typedef void (*setVertexBytesType)(id, SEL, const void *bytes, MTL::UInteger length, MTL::UInteger index);

typedef void (*setVertexBufferType)(id, SEL, cpp_mtl_Internal::Buffer buffer, MTL::UInteger offset, MTL::UInteger index);

typedef void (*setVertexBufferOffsetType)(id, SEL, MTL::UInteger offset, MTL::UInteger index);

typedef void (*setVertexTextureType)(id, SEL, cpp_mtl_Internal::Texture texture, MTL::UInteger index);

typedef void (*setFragmentBytesType)(id, SEL, const void *bytes, MTL::UInteger length, MTL::UInteger index);

typedef void (*setFragmentBufferType)(id, SEL, cpp_mtl_Internal::Buffer buffer, MTL::UInteger offset, MTL::UInteger index);

typedef void (*setFragmentBufferOffsetType)(id, SEL, MTL::UInteger offset, MTL::UInteger index);

typedef void (*setFragmentTextureType)(id, SEL, cpp_mtl_Internal::Texture texture, MTL::UInteger index);

struct RenderCommandEncoderDispatchTable {
    CPP_METAL_DECLARE_FUNCTION_POINTER(setVertexBytes);
    CPP_METAL_DECLARE_FUNCTION_POINTER(setVertexBuffer);
    CPP_METAL_DECLARE_FUNCTION_POINTER(setVertexBufferOffset);
    CPP_METAL_DECLARE_FUNCTION_POINTER(setVertexTexture);
    CPP_METAL_DECLARE_FUNCTION_POINTER(setFragmentBytes);
    CPP_METAL_DECLARE_FUNCTION_POINTER(setFragmentBuffer);
    CPP_METAL_DECLARE_FUNCTION_POINTER(setFragmentBufferOffset);
    CPP_METAL_DECLARE_FUNCTION_POINTER(setFragmentTexture);
    
    RenderCommandEncoderDispatchTable(cpp_mtl_Internal::ObjCObj *objCObj);
};

} // namespace cpp_mtl_Internal

#endif // cpp_mtl_RenderCommandEncoder_DispatchTable_hpp
