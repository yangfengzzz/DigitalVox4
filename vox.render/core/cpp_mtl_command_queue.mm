//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "cpp_mtl_command_queue.h"
#include "cpp_mtl_command_buffer.h"
#include "cpp_mtl_device.h"
#include "cpp_mtl_internal_macros.h"
#include "cpp_mtl_device_internals.h"

#include <Metal/Metal.h>

using namespace MTL;

CPP_METAL_CONSTRUCTOR_IMPLEMENTATION(CommandQueue);

CPP_METAL_NULL_REFERENCE_CONSTRUCTOR_IMPLEMENATATION(CommandQueue);

CPP_METAL_COPY_CONSTRUCTOR_AND_OPERATOR_OVERLOAD_IMPLEMENTATION(CommandQueue);

CommandQueue::~CommandQueue() {
    m_objCObj = nil;
}

CommandBuffer CommandQueue::commandBuffer() {
    const id <MTLCommandBuffer> objCObj = [m_objCObj commandBuffer];
    
    return CommandBuffer(objCObj, *m_device);
}

CPP_METAL_READWRITE_LABEL_PROPERTY_IMPLEMENTATION(CommandQueue);

CPP_METAL_DEVICE_GETTER_IMPLEMENTATION(CommandQueue);
