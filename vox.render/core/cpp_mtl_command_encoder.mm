//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "cpp_mtl_command_encoder.h"
#include "cpp_mtl_device.h"
#include <Metal/Metal.h>
#include "cpp_mtl_internal_macros.h"

using namespace MTL;

CPP_METAL_CONSTRUCTOR_IMPLEMENTATION(CommandEncoder);

CPP_METAL_COPY_CONSTRUCTOR_AND_OPERATOR_OVERLOAD_IMPLEMENTATION(CommandEncoder);

CommandEncoder::~CommandEncoder() {
    m_objCObj = nil;
}

CPP_METAL_READWRITE_LABEL_PROPERTY_IMPLEMENTATION(CommandEncoder);

CPP_METAL_DEVICE_GETTER_IMPLEMENTATION(CommandEncoder);

void CommandEncoder::pushDebugGroup(const CFStringRef string) {
    CPP_METAL_VALIDATE_WRAPPED_NIL();
    [m_objCObj pushDebugGroup:(__bridge NSString *) string];
}

void CommandEncoder::popDebugGroup() {
    CPP_METAL_VALIDATE_WRAPPED_NIL();
    [m_objCObj popDebugGroup];
}

void CommandEncoder::endEncoding() {
    CPP_METAL_VALIDATE_WRAPPED_NIL();
    [m_objCObj endEncoding];
}
