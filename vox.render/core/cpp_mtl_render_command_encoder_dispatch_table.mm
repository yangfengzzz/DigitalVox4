//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "cpp_mtl_render_command_encoder_dispatch_table.h"
#import <objc/runtime.h>

using namespace cpp_mtl_internal;

#define CPP_METAL_SET_IMPLEMENTATION(methodName) \
methodName = (methodName ## Type)[objCObj methodForSelector:methodName ## Sel]

RenderCommandEncoderDispatchTable::RenderCommandEncoderDispatchTable(NSObject *objCObj) {
    CPP_METAL_SET_IMPLEMENTATION(setVertexBytes);
    CPP_METAL_SET_IMPLEMENTATION(setVertexBuffer);
    CPP_METAL_SET_IMPLEMENTATION(setVertexBufferOffset);
    CPP_METAL_SET_IMPLEMENTATION(setVertexTexture);
    CPP_METAL_SET_IMPLEMENTATION(setFragmentBytes);
    CPP_METAL_SET_IMPLEMENTATION(setFragmentBuffer);
    CPP_METAL_SET_IMPLEMENTATION(setFragmentBufferOffset);
    CPP_METAL_SET_IMPLEMENTATION(setFragmentTexture);
}
