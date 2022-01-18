//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "cpp_mtl_command_buffer.h"
#include "cpp_mtl_render_command_encoder.h"
#include "cpp_mtl_internal_macros.h"
#include "cpp_mtl_render_pass.h"
#include "cpp_mtl_device.h"
#include "cpp_mtl_drawable.h"
#include <Metal/Metal.h>

using namespace MTL;

CommandBufferHandler::~CommandBufferHandler() {
    // Pure virtual class destructor
}

CPP_METAL_CONSTRUCTOR_IMPLEMENTATION(CommandBuffer);

CPP_METAL_COPY_CONSTRUCTOR_AND_OPERATOR_OVERLOAD_IMPLEMENTATION(CommandBuffer);

CommandBuffer::~CommandBuffer() {
    m_objCObj = nil;
}

bool CommandBuffer::operator==(const CommandBuffer &rhs) {
    return [m_objCObj isEqual:rhs.m_objCObj];
}

const Device* CommandBuffer::device() const {
    return m_device;
}

CPP_METAL_READWRITE_LABEL_PROPERTY_IMPLEMENTATION(CommandBuffer)

RenderCommandEncoder CommandBuffer::renderCommandEncoderWithDescriptor(const RenderPassDescriptor &descriptor) const {
    const id <MTLRenderCommandEncoder> objCObj = [m_objCObj renderCommandEncoderWithDescriptor:descriptor.objCObj()];
    
    return RenderCommandEncoder(objCObj, *m_device);
}

void CommandBuffer::commit() {
    CPP_METAL_VALIDATE_WRAPPED_NIL();
    
    [m_objCObj commit];
}

void CommandBuffer::presentDrawable(Drawable &drawable) {
    CPP_METAL_VALIDATE_WRAPPED_NIL();
    
    [m_objCObj presentDrawable:drawable.objCObj()];
    drawable.invalidate();
}

void CommandBuffer::waitUntilCompleted() {
    CPP_METAL_VALIDATE_WRAPPED_NIL();
    
    [m_objCObj waitUntilCompleted];
}

void CommandBuffer::addCompletedHandler(CommandBufferHandler &completedHandler) {
    Device *device = m_device;
    [m_objCObj addCompletedHandler:^(id <MTLCommandBuffer> _Nonnull objCCommandBuffer) {
        CommandBuffer commandBuffer(objCCommandBuffer, *device);
        completedHandler(commandBuffer);
    }];
}

void CommandBuffer::addScheduledHandler(CommandBufferHandler &scheduledHandler) {
    Device *device = m_device;
    [m_objCObj addScheduledHandler:^(id <MTLCommandBuffer> _Nonnull objCCommandBuffer) {
        CommandBuffer commandBuffer(objCCommandBuffer, *device);
        scheduledHandler(commandBuffer);
    }];
}
