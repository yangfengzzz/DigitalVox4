/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Implementation of C++ Metal render command encoder dispatch table class
*/

#include "CPPMetalRenderCommandEncoder_DispatchTable.hpp"
#import <objc/runtime.h>

using namespace CPPMetalInternal;

#define CPP_METAL_SET_IMPLEMENTATION(methodName) \
    methodName = (methodName ## Type)[objCObj methodForSelector:methodName ## Sel]

RenderCommandEncoderDispatchTable::RenderCommandEncoderDispatchTable(NSObject *objCObj)
{
    CPP_METAL_SET_IMPLEMENTATION( setVertexBytes );
    CPP_METAL_SET_IMPLEMENTATION( setVertexBuffer );
    CPP_METAL_SET_IMPLEMENTATION( setVertexBufferOffset );
    CPP_METAL_SET_IMPLEMENTATION( setVertexTexture );
    CPP_METAL_SET_IMPLEMENTATION( setFragmentBytes );
    CPP_METAL_SET_IMPLEMENTATION( setFragmentBuffer );
    CPP_METAL_SET_IMPLEMENTATION( setFragmentBufferOffset );
    CPP_METAL_SET_IMPLEMENTATION( setFragmentTexture );
}
